#include "machine.h"
#include "print.h"
#include "vcpu_timer.h"
#include "container_of.h"
#include "../bsp/clint.h"
#include "virt.h"
#include "string.h"
#include "clock.h"

static void clint_mmio_write(struct memory_region *region,
			     unsigned long addr, unsigned long val,
			     unsigned int len)
{
}

static unsigned long clint_mmio_read(struct memory_region *region,
				     unsigned long addr, unsigned int len)
{
	unsigned long reg = addr - region->start;
	struct vcpu *vcpu;

	vcpu = container_of(region->machine, struct vcpu, machine);
	if (!vcpu)
		return -1;

	if (reg == 0xbff8) {
		return vcpu_get_system_time(vcpu);
	}

	return 0;
}

static const struct memory_region_ops clint_memory_ops = {
	.write = clint_mmio_write,
	.read = clint_mmio_read,
};

int create_clint_device(struct virt_machine *machine, int id,
			unsigned long base, unsigned int size)
{
	add_memory_region(machine, id, base, size, &clint_memory_ops);

	return 0;
}

int create_clint_priv_data(void *ptr)
{
	struct clint_data data;
	int len = sizeof(data);

	data.clint_freq = get_system_clock_freq();
	memcpy((char *)ptr, (char *)&data, len);

	return len;
}
