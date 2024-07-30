#include "device.h"
#include <asm/pgtable.h>
#include <asm/type.h>
#include "mm.h"
#include "vmap.h"
#include "print.h"
#include "page_tlb_test.h"

static unsigned long memory_test_va, memory_test_va1;

unsigned long get_memory_test_addr(char addr_num)
{
	if (addr_num == FIRST_MAPPING_ADDR)
		return memory_test_va;
	else if (addr_num == SECOND_MAPPING_ADDR)
		return memory_test_va1;
	return 0;
}

int page_tlb_test_init(unsigned long base, unsigned int len, void *data)
{
	pgprot_t pgprot;
	unsigned long va, va1;

	va = (unsigned long)vmap_alloc(len);
	va1 = va + 64;
	pgprot = __pgprot(_PAGE_BASE | _PAGE_READ | _PAGE_WRITE | _PAGE_DIRTY);

	if (mmu_page_mapping(base, va, len, pgprot))
		return -1;
	if (mmu_page_mapping(base, va1, len, pgprot))
		return -1;

	memory_test_va = va;
	memory_test_va1 = va1;

	print("va:%s, va1:%s\n", va, va1);

	return 0;
}

DRIVER_REGISTER(page_tlb_test, page_tlb_test_init, "memory-test");
