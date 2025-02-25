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

#ifndef __VIRT_MACHINE_H
#define __VIRT_MACHINE_H

#include "list.h"
#include "spinlocks.h"
#include "device.h"

struct memory_region;
struct virt_machine;

enum {
	VIRT_MEMORY = 0,
	VIRT_UART,
	VIRT_SRAM,
	VIRT_TEST,
	VIRT_CLINT,
	VIRT_IMSIC,
	VIRT_SCHEDULER,
	VIRT_PCI_CONFIG,
	VIRT_PCI_MMIO,
};

struct memory_region_ops {
	void (*write)(struct memory_region * region, unsigned long addr,
		      unsigned long val, unsigned int len);
	unsigned long (*read)(struct memory_region * region, unsigned long addr,
			      unsigned int len);
};

struct virt_machine_memmap {
	unsigned long base;
	unsigned int size;
};

struct memory_region {
	struct list_head list;
	int id;
	unsigned long start;
	unsigned long end;
	unsigned long hpa_base;
	const struct memory_region_ops *ops;
	struct virt_machine *machine;
};

struct virt_machine {
	struct list_head memory_region_list;
	spinlock_t lock;
	struct device_init_entry *device_entry;
	struct device_init_entry *device_entry_host_va;
	void *entry_data;
	int entry_data_len;
	void *entry_data_gpa;
	unsigned int device_entry_count;
	unsigned long gstage_pgdp;
	struct list_head extern_device_entry_list;
	struct list_head pt_devices_res;
	struct pci_generic_emulator *pci_emu;
};

struct extern_device_init_entry {
	struct list_head list;
	struct device_init_entry entry;
	struct memory_region_ops *ops;
};

static inline unsigned int machine_get_memory_test_size(struct virt_machine
							*machine)
{
	struct memory_region *entry;
	unsigned int ret = 0;

	list_for_each_entry(entry, &machine->memory_region_list, list) {
		if (entry->id == VIRT_TEST)
			ret = entry->end - entry->start;
	}

	return ret;
}

static inline unsigned long machine_get_memory_test_start(struct virt_machine
							  *machine)
{
	struct memory_region *entry;
	unsigned long ret = (-1UL);

	list_for_each_entry(entry, &machine->memory_region_list, list) {
		if (entry->id == VIRT_TEST)
			ret = entry->start;
	}

	return ret;
}

static inline unsigned int machine_get_sram_size(struct virt_machine *machine)
{
	struct memory_region *entry;
	unsigned int ret = 0;

	list_for_each_entry(entry, &machine->memory_region_list, list) {
		if (entry->id == VIRT_SRAM)
			ret = entry->end - entry->start;
	}

	return ret;
}

static inline unsigned long machine_get_sram_start(struct virt_machine *machine)
{
	struct memory_region *entry;
	unsigned long ret = (-1UL);

	list_for_each_entry(entry, &machine->memory_region_list, list) {
		if (entry->id == VIRT_SRAM)
			ret = entry->start;
	}

	return ret;
}

static inline unsigned int machine_get_ddr_size(struct virt_machine *machine)
{
	struct memory_region *entry;
	unsigned int ret = 0;

	list_for_each_entry(entry, &machine->memory_region_list, list) {
		if (entry->id == VIRT_MEMORY)
			ret = entry->end - entry->start;
	}

	return ret;
}

static inline unsigned long machine_get_ddr_start(struct virt_machine *machine)
{
	struct memory_region *entry;
	unsigned long ret = (-1UL);

	list_for_each_entry(entry, &machine->memory_region_list, list) {
		if (entry->id == VIRT_MEMORY)
			ret = entry->start;
	}

	return ret;
}

int machine_init(struct virt_machine *machine);
int machine_finialize(struct virt_machine *machine);
int add_memory_region(struct virt_machine *machine, int id, unsigned long base,
		      unsigned int size, const struct memory_region_ops *ops);
struct memory_region *find_memory_region_by_id(struct virt_machine *machine,
					       int id);
struct memory_region *find_memory_region(struct virt_machine *machine,
					 unsigned long gpa);
void device_entry_data_redirect(struct virt_machine *machine);
unsigned long get_machine_memmap_base(int id);
unsigned int get_machine_memmap_size(int id);
void machine_extern_device_entry_register(struct list_head *l,
					  struct extern_device_init_entry *entry);

#endif
