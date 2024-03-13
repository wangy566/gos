#include <device.h>
#include <string.h>
#include <trap.h>
#include <print.h>
#include <asm/type.h>
#include <irq.h>
#include <timer.h>
#include "mm.h"

static LIST_HEAD(irq_domains);

static struct irq_domain intc_domain;

struct irq_domain *find_irq_domain(char *name)
{
	struct irq_domain *domain;

	list_for_each_entry(domain, &irq_domains, list)
	    if (!strncmp(domain->name, name, 128))
		return domain;

	return NULL;
}

struct irq_info *find_irq_info(struct irq_domain *domain, int hwirq)
{
	struct irq_info *info;

	if (!domain)
		return NULL;

	list_for_each_entry(info, &domain->irq_info_head, list)
	    if (info->hwirq == hwirq)
		return info;

	return NULL;
}

void handle_irq(unsigned long cause)
{
	struct irq_domain *d = find_irq_domain("INTC");
	struct irq_info *irq_info;

	if (!d) {
		print("unsupported cause: %d\n", cause & (~SCAUSE_IRQ));
		while (1) ;
		return;
	}

	irq_info = find_irq_info(d, cause & (~SCAUSE_IRQ));
	if (!irq_info || !irq_info->handler)
		return;

	irq_info->handler(irq_info->priv);
}

int irqchip_setup(struct device_init_entry *hw)
{
	extern unsigned long IRQCHIP_INIT_TABLE, IRQCHIP_INIT_TABLE_END;
	int driver_nr =
	    (struct irqchip_init_entry *)&IRQCHIP_INIT_TABLE_END -
	    (struct irqchip_init_entry *)&IRQCHIP_INIT_TABLE;
	int driver_nr_tmp = 0;
	struct irqchip_init_entry *driver_entry;
	struct device_init_entry *device_entry = hw;
	struct irqchip_init_entry *driver_tmp =
	    (struct irqchip_init_entry *)&IRQCHIP_INIT_TABLE;
	struct irq_domain *d;

	while (strncmp(device_entry->compatible, "THE END", sizeof("THE_END"))) {
		driver_nr_tmp = driver_nr;
		for (driver_entry = driver_tmp; driver_nr_tmp;
		     driver_entry++, driver_nr_tmp--) {
			d = find_irq_domain(device_entry->irq_parent);
			if (!strncmp
			    (driver_entry->compatible, device_entry->compatible,
			     128)) {
				driver_entry->init(device_entry->compatible,
						   device_entry->start, d,
						   device_entry->data);
			}
		}
		device_entry++;
	}

	return 0;
}

int domain_handle_irq(struct irq_domain *domain, unsigned int hwirq, void *data)
{
	struct irq_info *irq_info;
	if (!domain->parent_domain)
		return -1;

	irq_info = find_irq_info(domain, hwirq);
	if (!irq_info || !irq_info->handler)
		return -1;

	irq_info->handler(irq_info->priv);

	return 0;
}

int get_hwirq(struct device *dev, int *ret_irq)
{
	struct irq_domain *irq_domain = dev->irq_domain;
	int *irqs = dev->irqs, i, num, hwirq;

	if (dev->irq_num > MAX_IRQ_NUM)
		num = MAX_IRQ_NUM;
	else
		num = dev->irq_num;

	if (!irqs)
		return -1;

	for (i = 0; i < num; i++)
		ret_irq[i] = irqs[i];

	if (!irq_domain->domain_ops || !irq_domain->domain_ops->alloc_irqs)
		return num;

	hwirq = irq_domain->domain_ops->alloc_irqs(num, irq_domain->priv);
	if (hwirq == -1)
		return 0;

	for (i = 0; i < num; i++) {
		domain_activate_irq(irq_domain, hwirq + i, NULL);
	}

	return num;
}

int msi_get_hwirq(struct device *dev, int nr_irqs,
		  write_msi_msg_t write_msi_msg)
{
	struct irq_domain *irq_domain = dev->irq_domain;
	int i, hwirq;

	if (!irq_domain->domain_ops->alloc_irqs)
		return 0;

	hwirq = irq_domain->domain_ops->alloc_irqs(nr_irqs, irq_domain->priv);
	if (hwirq == -1)
		return 0;

	for (i = 0; i < nr_irqs; i++)
		domain_activate_irq(irq_domain, hwirq + i, write_msi_msg);

	return hwirq;
}

int register_device_irq(struct irq_domain *domain, unsigned int hwirq,
			void (*handler)(void *data), void *priv)
{
	struct irq_info *irq_info = NULL;

	irq_info = find_irq_info(domain, hwirq);
	if (!irq_info) {
		irq_info = (struct irq_info *)mm_alloc(sizeof(struct irq_info));
		if (!irq_info) {
			print("%s -- Out of memory\n", __FUNCTION__);
			return -1;
		}
	}

	irq_info->hwirq = hwirq;
	irq_info->handler = handler;
	irq_info->priv = priv;
	list_add(&irq_info->list, &domain->irq_info_head);

	if (domain->domain_ops && domain->domain_ops->unmask_irq)
		domain->domain_ops->unmask_irq(hwirq, domain->priv);

	return 0;
}

int domain_activate_irq(struct irq_domain *domain, int hwirq,
			write_msi_msg_t write_msi_msg)
{
	struct irq_domain *p_domain = domain;
	unsigned long msi_addr = 0, msi_data = 0;

	while (p_domain) {
		if (p_domain && p_domain->domain_ops->get_msi_msg)
			break;

		p_domain = p_domain->parent_domain;
	}

	if (p_domain && p_domain->domain_ops->get_msi_msg)
		p_domain->domain_ops->get_msi_msg(p_domain, hwirq,
						  &msi_addr, &msi_data,
						  p_domain->priv);

	if (write_msi_msg) {
		write_msi_msg(msi_addr, msi_data, hwirq, NULL);
		return 0;
	}

	p_domain = domain;
	while (p_domain) {
		if (p_domain && p_domain->write_msi_msg)
			break;

		p_domain = p_domain->parent_domain;
	}

	if (p_domain && p_domain->write_msi_msg)
		p_domain->write_msi_msg(msi_addr, msi_data, hwirq,
					domain->priv);

	return 0;
}

int irq_domain_init(struct irq_domain *domain, char *name,
		    struct irq_domain_ops *ops,
		    struct irq_domain *parent, void *priv)
{
	domain->parent_domain = parent;
	strcpy(domain->name, name);
	INIT_LIST_HEAD(&domain->irq_info_head);
	list_add(&domain->list, &irq_domains);
	domain->priv = priv;
	domain->domain_ops = ops;

	return 0;
}

int msi_domain_init(struct irq_domain *domain, char *name,
		    struct irq_domain_ops *ops, struct irq_domain *parent,
		    write_msi_msg_t write_msi_msg, void *priv)
{
	domain->write_msi_msg = write_msi_msg;

	return irq_domain_init(domain, name, ops, parent, priv);
}

int irq_domain_init_hierarchy(struct irq_domain *domain, char *name,
			      struct irq_domain_ops *ops,
			      struct irq_domain *parent, unsigned int hwirq,
			      void (*handler)(void *data), void *priv)
{
	register_device_irq(parent, hwirq, handler, priv);

	return irq_domain_init(domain, name, ops, parent, priv);
}

int irq_init(void)
{
	return irq_domain_init(&intc_domain, "INTC", NULL, NULL, NULL);
}
