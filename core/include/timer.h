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

#ifndef _TIMER_H
#define _TIMER_H

#include <device.h>
#include "list.h"

struct timer_event_info {
	struct list_head list;
	void (*handler)(void *data);
	void (*del_cb)(void *data);
	void *data;
	int done;
	unsigned long expiry_time;
	unsigned long period;
	int restart;
	int freeze;
};

int init_timer(struct device_init_entry *hw);
unsigned long get_system_time(void);
unsigned long get_system_time_ms(void);
struct timer_event_info *set_timer(unsigned long ms, void (*timer_handler)(void *data), void *data);
struct timer_event_info *set_timer_restart(unsigned long ms, void (*timer_handler)(void *data), void *data);
struct timer_event_info *set_timer_cpu(unsigned long ms,
				void (*timer_handler)(void *data), void *data, int cpu);
struct timer_event_info *set_timer_restart_cpu(unsigned long ms,
				void (*timer_handler)(void *data), void *data, int cpu);
int del_timer_cpu(struct timer_event_info *timer, int cpu);
int del_timer(struct timer_event_info *timer);
void set_timer_freeze(struct timer_event_info *timer, int freeze);

#endif
