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

#ifndef _MM_H
#define _MM_H

#include <device.h>
#include <asm/pgtable.h>


#define GFP_NOCACHE (1UL << 0)
#define GFP_IO      (1UL << 1)

#define MAX_BYTE_PER_MAPS (2*1024*1024*1024UL)
struct mem_maps {
	unsigned long maps[8192];	//2G
	unsigned map_nr;
};

#define MAX_MEM_BLOCK_COUNT 16
#define MAX_MEM_BLOCK_RESV_COUNT 16
struct memory_block {
	unsigned long memory_block_start[MAX_MEM_BLOCK_COUNT];
	unsigned int memory_block_size[MAX_MEM_BLOCK_COUNT];
	int avail;
	struct mem_maps maps[MAX_MEM_BLOCK_COUNT];
	int max_byte_per_maps;
	unsigned long memory_block_resv_start[MAX_MEM_BLOCK_RESV_COUNT];
	unsigned int memory_block_resv_size[MAX_MEM_BLOCK_RESV_COUNT];
	int reserved_cnt;
};

#define for_each_memory_block(entry, blocks, n)      \
	for (entry = (struct memory_block *)blocks; n > 0; entry++, n--)

int paging_init(struct device_init_entry *hw);
void enable_mmu(int on);
int mmu_page_mapping_2M(unsigned long phy, unsigned long virt, unsigned int size,
		        pgprot_t pgprot);
int mmu_page_mapping_1G(unsigned long phy, unsigned long virt, unsigned int size,
		        pgprot_t pgprot);
int mmu_page_mapping_8k(unsigned long phy, unsigned long virt, unsigned int size,
			 pgprot_t pgprot);
int mmu_page_mapping_16k(unsigned long phy, unsigned long virt, unsigned int size,
			 pgprot_t pgprot);
int mmu_page_mapping_32k(unsigned long phy, unsigned long virt, unsigned int size,
			 pgprot_t pgprot);
int mmu_page_mapping_64k(unsigned long phy, unsigned long virt, unsigned int size,
			 pgprot_t pgprot);
int mmu_page_mapping(unsigned long phy, unsigned long virt, unsigned int size,
		     pgprot_t pgprot);
int mmu_page_mapping_no_sfence(unsigned long phy, unsigned long virt, unsigned int size,
			       pgprot_t pgprot);
int mmu_user_page_mapping(unsigned long *pgdp, unsigned long phy,
			  unsigned long virt, unsigned int size,
			  pgprot_t pgprot);
int mmu_gstage_page_mapping(unsigned long *_pgdp, unsigned long phy,
			    unsigned long virt, unsigned int size,
			    pgprot_t pgprot);
int mmu_gstage_page_mapping_2M(unsigned long *_pgdp, unsigned long phy,
			       unsigned long virt, unsigned int size,
			       pgprot_t pgprot);
int mmu_gstage_page_mapping_1G(unsigned long *_pgdp, unsigned long phy,
			       unsigned long virt, unsigned int size,
			       pgprot_t pgprot);
int mmu_page_mapping_lazy(unsigned long virt, unsigned int size,
			  pgprot_t pgprot);
void *walk_pt_va_to_pa(unsigned long va);
void *walk_pt_va_to_pa_huge(unsigned long va, int page_size);
void mm_init(struct device_init_entry *hw);
void *mm_alloc_align(unsigned long align, unsigned int size);
void *__mm_alloc(unsigned int size);
void *mm_alloc(unsigned int size);
void *mm_alloc_fix(unsigned long addr, unsigned int size);
void __mm_free(void *addr, unsigned int size);
void mm_free(void *addr, unsigned int size);
unsigned long alloc_zero_page(int gfp);
unsigned long get_phy_start(void);
unsigned long get_phy_end(void);
struct memory_block *get_mm_blocks(void);
int do_page_fault(unsigned long addr);
unsigned long *mmu_get_pte_level(unsigned long virt_addr, int lvl);
void mmu_walk_and_print_pte(unsigned long virt_addr);
unsigned long *mmu_get_pte(unsigned long virt_addr);
unsigned long get_default_pgd(void);
void walk_unused_mem_and_print(void);
void unused_mem_walk(void (*fn)(unsigned long addr, unsigned int nr, void *data), void *data);
int mem_range_is_free(unsigned long addr, unsigned int size);
void reserved_mem_walk(void (*fn)(unsigned long addr, unsigned int nr, void *data), void *data);

static inline void dump_fault_addr_pt(unsigned long addr)
{
	return mmu_walk_and_print_pte(addr);
}

#endif
