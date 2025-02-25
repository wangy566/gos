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

#ifndef SBI_CLINT_H
#define SBI_CLINT_H

#include "sbi_trap.h"

void sbi_timer_process(void);
void clint_timer_event_start(struct sbi_trap_hw_context *ctx,
			     unsigned long next_event);
int sbi_clint_init(unsigned int hart_id, struct sbi_trap_hw_context *ctx);

#endif
