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

#include <asm/type.h>
#include <print.h>
#include <device.h>
#include "../command.h"
#include "vmap.h"
#include "asm/pgtable.h"
#include "string.h"
#include "mm.h"
#include "asm/tlbflush.h"

static int cmd_svnapot_test_handler(int argc, char *argv[], void *priv)
{
	char *addr;
	int size = PAGE_64K_SIZE, i;
	int pnum = PAGE_64K_SIZE / PAGE_SIZE;
	unsigned long *pte;

	addr = (char *)vmem_alloc_huge(size, PAGE_64K_SIZE, NULL);
	addr[0] = 66;
	addr[size - 1] = 88;

	for (i = 0; i < pnum; i++) {
		addr[i * PAGE_SIZE] = 66;
		addr[i * PAGE_SIZE + PAGE_SIZE - 1] = 88;
		pte = mmu_get_pte((unsigned long)(addr + i * PAGE_SIZE));
		print("pte:0x%lx\n", *pte);
		print("addr[%d]:%d addr[%d]:%d\n",
			i * PAGE_SIZE, addr[i * PAGE_SIZE],
			i * PAGE_SIZE + PAGE_SIZE - 1,
			addr[i * PAGE_SIZE + PAGE_SIZE - 1]);
	}

	print("TEST PASS\n");

	return 0;
}

static const struct command cmd_svnapot_test = {
	.cmd = "svnapot_test",
	.handler = cmd_svnapot_test_handler,
	.priv = NULL,
};

int svnapot_test_init()
{
	register_command(&cmd_svnapot_test);

	return 0;
}

APP_COMMAND_REGISTER(svnapot_test, svnapot_test_init);
