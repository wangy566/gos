#include <string.h>
#include <print.h>
#include <device.h>
#include <asm/mmio.h>
#include <asm/csr.h>
#include "sbi_trap.h"

static unsigned long get_clint_base(void)
{
	extern unsigned long DEVICE_INIT_TABLE, DEVICE_INIT_TABLE_END;
	int nr = &DEVICE_INIT_TABLE_END - &DEVICE_INIT_TABLE;
	struct device_init_entry *device_entry;

	for (device_entry = (struct device_init_entry *)&DEVICE_INIT_TABLE;
	     nr; device_entry++, nr--) {
		if (!strncmp(device_entry->compatible, "clint", 128)) {
			return device_entry->start;
		}
	}

	return 0;
}

void sbi_timer_process(void)
{
	csr_clear(mie, MIP_MTIP);
	//csr_clear(mip, MIP_MTIP);
	csr_set(mip, MIP_STIP);
}

void clint_timer_event_start(struct sbi_trap_hw_context *ctx,
			     unsigned long next_event)
{
	writeq(ctx->timer_cmp, next_event);

	csr_clear(mip, MIP_STIP);
	csr_set(mie, MIP_MTIP);
}

int sbi_clint_init(unsigned int hart_id, struct sbi_trap_hw_context *ctx)
{
	int rc = 0;
	unsigned long base;

	base = get_clint_base();
	sbi_print("clint base: 0x%x\n", base);

	ctx->timer_cmp = base + 0x4000 + hart_id * 8;
	ctx->ipi = base + hart_id * 4;

	return rc;
}
