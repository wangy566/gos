#ifndef _MM_H
#define _MM_H

#include <device.h>
#include <asm/pgtable.h>

#define MAX_BYTE_PER_MAPS (2*1024*1024*1024UL)
struct mem_maps {
	unsigned long maps[8192];	//2G
	unsigned map_nr;
};

#define MAX_MEM_BLOCK_COUNT 16
struct memory_block {
	unsigned long memory_block_start[MAX_MEM_BLOCK_COUNT];
	unsigned int memory_block_size[MAX_MEM_BLOCK_COUNT];
	int avail;
	struct mem_maps maps[MAX_MEM_BLOCK_COUNT];
	int max_byte_per_maps;
};

#define for_each_memory_block(entry, blocks, n)      \
	for (entry = (struct memory_block *)blocks; n > 0; entry++, n--)

int paging_init(struct device_init_entry *hw);
void enable_mmu(int on);
int mmu_page_mapping(unsigned long phy, unsigned long virt, unsigned int size,
		     pgprot_t pgprot);
int mmu_page_mapping_no_sfence(unsigned long phy, unsigned long virt, unsigned int size,
			       pgprot_t pgprot);
int mmu_user_page_mapping(unsigned long phy, unsigned long virt,
			  unsigned int size, pgprot_t pgprot);
int mmu_gstage_page_mapping(unsigned long *_pgdp, unsigned long phy,
			    unsigned long virt, unsigned int size,
			    pgprot_t pgprot);
int mmu_page_mapping_lazy(unsigned long virt, unsigned int size,
			  pgprot_t pgprot);
void *walk_pt_va_to_pa(unsigned long va);
void mm_init(struct device_init_entry *hw);
void *mm_alloc(unsigned int size);
void mm_free(void *addr, unsigned int size);
unsigned long alloc_zero_page(int gfp);
unsigned long get_phy_start(void);
unsigned long get_phy_end(void);
struct memory_block *get_mm_blocks(void);
int do_page_fault(unsigned long addr);
unsigned long *mmu_get_pte(unsigned long virt_addr);
unsigned long get_default_pgd(void);

#endif
