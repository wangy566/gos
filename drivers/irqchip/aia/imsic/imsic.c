/*
 * Copyright (c) 2024 Beijing Institute of Open Source Chip (BOSC)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <print.h>
#include <string.h>
#include <asm/trap.h>
#include <asm/csr.h>
#include "mm.h"
#include "device.h"
#include "imsic.h"
#include "imsic_reg.h"
#include "cpu.h"
#include "vmap.h"

static struct imsic imsic;
extern int mmu_is_on;

static int imsic_id_get_target(struct imsic *p_imsic, int id)
{
	return p_imsic->ids_target_cpu[id];
}

static int imsic_get_msi_msg(struct irq_domain *domain, int hwirq,
			     unsigned long *msi_addr, unsigned long *msi_data,
			     void *data)
{
	struct imsic *p_imsic = (struct imsic *)data;
	unsigned int cpu;

	cpu = imsic_id_get_target(p_imsic, hwirq);
	if (cpu == ~0U)
		return -1;

	*msi_addr = p_imsic->interrupt_file_base[cpu][0];
	*msi_data = hwirq;

	return 0;
}

struct irq_domain *imsic_get_irq_domain()
{
	return &imsic.irq_domain;
}

unsigned long imsic_get_interrupt_file_base(int cpu, int guest_id)
{
	if (guest_id > imsic.nr_guests - 1)
		return -1;

	return imsic.interrupt_file_base[cpu][guest_id];
}

int imsic_get_hart_index_bits()
{
	return imsic.hart_index_bits;
}

int imsic_get_group_index_bits()
{
	return imsic.group_index_bits;
}

void imsic_ids_local_delivery(int enable)
{
	if (enable) {
		imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);
		imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);
	} else {
		imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_DISABLE_EIDELIVERY);
		imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_DISABLE_EITHRESHOLD);
	}
}

static int imsic_alloc_ids_fix(struct imsic *p_imsic, int id)
{
	unsigned long ids;
	int per_ids = sizeof(unsigned long) * 8;

	ids = p_imsic->ids_used_bits[id / per_ids];
	if (((ids >> (id % per_ids)) & 0x1UL) == 0) {
		ids |= (1 << (id % per_ids));
		p_imsic->ids_used_bits[id / per_ids] = ids;
		return id;
	}

	return -1;
}

static int imsic_alloc_ids(int nr_irqs, struct imsic *p_imsic)
{
	int index = 0, nr = 0;
	unsigned long ids;
	int per_ids = sizeof(unsigned long) * 8;

	while (index < p_imsic->nr_ids) {
		ids = p_imsic->ids_used_bits[index / per_ids];
		if (((ids >> (index % per_ids)) & 0x1UL) == 0) {
			if (++nr == nr_irqs)
				goto alloc_success;
		} else {
			nr = 0;
		}

		index++;
	}

	return -1;

alloc_success:
	ids |= (1 << (index % per_ids));
	p_imsic->ids_used_bits[index / per_ids] = ids;

	return (index - nr_irqs + 1);
}

static int imsic_disable_id(struct imsic *p_imsic, int id)
{
	unsigned long ids;
	unsigned long *ids_enable_bits = p_imsic->ids_enable_bits;
	int bits_per_ids = sizeof(unsigned long) * 8;

	ids = ids_enable_bits[id / bits_per_ids];
	ids &= ~(1 << (id % bits_per_ids));
	ids_enable_bits[id / bits_per_ids] = ids;

	imsic_id_disable(id);

	return 0;
}

static int imsic_enable_id(struct imsic *p_imsic, int id)
{
	unsigned long ids;
	unsigned long *ids_enable_bits = p_imsic->ids_enable_bits;
	int bits_per_ids = sizeof(unsigned long) * 8;

	ids = ids_enable_bits[id / bits_per_ids];
	ids |= (1 << (id % bits_per_ids));
	ids_enable_bits[id / bits_per_ids] = ids;

	imsic_id_enable(id);

	return 0;
}

static int imsic_id_set_target(struct imsic *p_imsic, int id, int cpu)
{
	if (id > MAX_IDS)
		return -1;

	p_imsic->ids_target_cpu[id] = cpu;

	return 0;
}

static int imsic_state_setup(struct imsic *p_imsic,
			     struct imsic_priv_data *data)
{
	int hart_id, guest_id;
	int i;

	print
	    ("imsic: nr_harts:%d, nr_guests:%d, nr_ids:%d, guest_index_bits:%d, hart_index_bits:%d, group_index_bits:%d\n",
	     data->nr_harts, data->nr_guests, data->nr_ids,
	     data->guest_index_bits, data->hart_index_bits,
	     data->group_index_bits);

	p_imsic->guest_index_bits = data->guest_index_bits;
	p_imsic->hart_index_bits = data->hart_index_bits;
	p_imsic->group_index_bits = data->group_index_bits;

	p_imsic->nr_harts = data->nr_harts;
	p_imsic->nr_guests = data->nr_guests;
	p_imsic->nr_ids = data->nr_ids;

	if ((1ULL << p_imsic->guest_index_bits) < p_imsic->nr_guests) {
		print("imsic: invalid guest param\n");
		return -1;
	}

	if ((1ULL << p_imsic->hart_index_bits) < p_imsic->nr_harts) {
		print("imsic: invalid hart param\n");
		return -1;
	}

	for (hart_id = 0; hart_id < p_imsic->nr_harts; hart_id++) {
		for (guest_id = 0; guest_id < p_imsic->nr_guests; guest_id++) {
			unsigned long per_hart_base;
			unsigned long interrupt_file_base;

			if (mmu_is_on)
				per_hart_base =
					(unsigned long)walk_pt_va_to_pa(p_imsic->base) +
					(1ULL << (p_imsic->guest_index_bits + 12)) *
					hart_id;
			else
				per_hart_base = p_imsic->base + (1ULL << (p_imsic->guest_index_bits + 12)) * hart_id;

			interrupt_file_base =
			    per_hart_base + (1ULL << 12) * guest_id;
			p_imsic->interrupt_file_base[hart_id][guest_id] =
			    interrupt_file_base;
			print("imsic: interrupt file(hart_%d, guest_%d): 0x%x\n",
			      hart_id, guest_id,
			      interrupt_file_base);
		}
	}

	memset((char *)p_imsic->ids_enable_bits, 0,
	       sizeof(p_imsic->ids_enable_bits) /
	       sizeof(p_imsic->ids_enable_bits[0]) * sizeof(unsigned long));
	memset((char *)p_imsic->ids_used_bits, 0,
	       sizeof(p_imsic->ids_used_bits) /
	       sizeof(p_imsic->ids_used_bits[0]) * sizeof(unsigned long));

	for (i = 0;
	     i <
	     sizeof(p_imsic->ids_target_cpu) /
	     sizeof(p_imsic->ids_target_cpu[0]); i++) {
		p_imsic->ids_target_cpu[i] = ~0U;
	}

	imsic_alloc_ids_fix(p_imsic, 0);	//interrupt identity 0 is unused

	return 0;
}

static void imsic_handle_irq(void *data)
{
	int hwirq;
	struct imsic *p_imsic = (struct imsic *)data;

	while ((hwirq = csr_swap(CSR_TOPEI, 0))) {
		hwirq = hwirq >> TOPEI_ID_SHIFT;
		domain_handle_irq(&p_imsic->irq_domain, hwirq, NULL);
	}
}

static int imsic_alloc_irqs(int nr_irqs, void *data)
{
	struct imsic *p_imsic = (struct imsic *)data;
	int id = -1, i;

	id = imsic_alloc_ids(nr_irqs, p_imsic);
	if (id == -1)
		return -1;

	for (i = 0; i < nr_irqs; i++) {
		// set target cpus of all cpus to cpu0 default
		imsic_id_set_target(p_imsic, id + i, 0);
	}

	return id;
}

static int imsic_mask_irq(int hwirq, void *data)
{
	struct imsic *p_imsic = (struct imsic *)data;

	return imsic_disable_id(p_imsic, hwirq);
}

static int imsic_unmask_irq(int hwirq, void *data)
{
	struct imsic *p_imsic = (struct imsic *)data;

	return imsic_enable_id(p_imsic, hwirq);
}

static int imsic_set_affinity(int hwirq, int cpu)
{
	return imsic_id_set_target(&imsic, hwirq, cpu);
}

static struct irq_domain_ops imsic_irq_domain_ops = {
	.alloc_irqs = imsic_alloc_irqs,
	.mask_irq = imsic_mask_irq,
	.unmask_irq = imsic_unmask_irq,
	.get_msi_msg = imsic_get_msi_msg,
	.set_affinity = imsic_set_affinity,
};

static int imsic_cpuhp_startup(struct cpu_hotplug_notifier *notifier, int cpu)
{
	int i;

	imsic_ids_local_delivery(1);

	for (i = 0; i < imsic.nr_ids; i++)
		imsic_enable_id(&imsic, i);

	return 0;
}

static struct cpu_hotplug_notifier imsic_cpuhp_notifier = {
	.startup = imsic_cpuhp_startup,
};

int imsic_init(char *name, unsigned long base, int len,
	       struct irq_domain *parent, void *data)
{
	int i;
	struct imsic_priv_data *state = (struct imsic_priv_data *)data;
	unsigned long addr;

	memset((char *)&imsic, 0, sizeof(struct imsic));

	addr = (unsigned long)ioremap((void *)base, len, 0);

	imsic.base = addr;
	imsic_state_setup(&imsic, state);

	irq_domain_init_cascade(&imsic.irq_domain, name,
				&imsic_irq_domain_ops, parent,
				INTERRUPT_CAUSE_EXTERNAL, imsic_handle_irq,
				&imsic);

	imsic_ids_local_delivery(1);

	for (i = 0; i < imsic.nr_ids; i++)
		imsic_enable_id(&imsic, i);

	cpu_hotplug_notify_register(&imsic_cpuhp_notifier);

	print("imsic: init success irq_domain:0x%x\n", &imsic.irq_domain);

	return 0;
}

IRQCHIP_REGISTER(imsic, imsic_init, "IMSIC");
