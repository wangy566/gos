#include <device.h>
#include <irq.h>
#include <asm/type.h>
#include "timer.h"
#include "string.h"
#include "cpu.h"
#include "mm.h"
#include "clock.h"
#include "vmap.h"
#include "asm/sbi.h"

extern int mmu_is_on;

unsigned long get_system_time()
{
	return get_clocksource_counter();
}

unsigned long get_system_time_ms()
{
	return get_clocksource_counter();
}

static void timer_del_handler(void *self)
{
	struct timer_event_info *t = (struct timer_event_info *)self;

	mm_free(t, sizeof(struct timer_event_info));
}

static int __set_timer(unsigned long ms, void (*timer_handler)(void *data),
		       void *data, int restart, int cpu)
{
	struct timer_event_info *timer;

	timer = mm_alloc(sizeof(struct timer_event_info));

	if (!timer)
		return -1;

	timer->handler = timer_handler;
	timer->del_cb = timer_del_handler;
	timer->data = data;
	timer->expiry_time = ms + get_system_time();
	timer->restart = restart;

	if (restart)
		timer->period = ms;

	return register_timer_event(timer, cpu);
}

int set_timer(unsigned long ms, void (*timer_handler)(void *data), void *data)
{
	return __set_timer(ms, timer_handler, data, 0, sbi_get_cpu_id());
}

int set_timer_restart(unsigned long ms, void (*timer_handler)(void *data), void *data)
{
	return __set_timer(ms, timer_handler, data, 1, sbi_get_cpu_id());
}

int set_timer_cpu(unsigned long ms, void (*timer_handler)(void *data),
	      void *data, int cpu)
{
	return __set_timer(ms, timer_handler, data, 0, cpu);
}

int set_timer_restart_cpu(unsigned long ms, void (*timer_handler)(void *data),
		      void *data, int cpu)
{
	return __set_timer(ms, timer_handler, data, 1, cpu);
}

static int timer_setup(struct device_init_entry *hw)
{
	extern unsigned long TIMER_INIT_TABLE, TIMER_INIT_TABLE_END;
	int driver_nr =
	    (struct timer_init_entry *)&TIMER_INIT_TABLE_END -
	    (struct timer_init_entry *)&TIMER_INIT_TABLE;
	int driver_nr_tmp = 0;
	struct timer_init_entry *driver_entry;
	struct device_init_entry *device_entry = hw;
	struct timer_init_entry *driver_tmp =
	    (struct timer_init_entry *)&TIMER_INIT_TABLE;
	struct irq_domain *d;
	unsigned long base;

	while (strncmp(device_entry->compatible, "THE END", sizeof("THE END"))) {
		driver_nr_tmp = driver_nr;
		for (driver_entry = driver_tmp; driver_nr_tmp;
		     driver_entry++, driver_nr_tmp--) {
			if (!strncmp
			    (driver_entry->compatible, device_entry->compatible,
			     128)) {
				d = find_irq_domain(device_entry->irq_parent);
				if (mmu_is_on)
					base = (unsigned long)ioremap((void *)
								      device_entry->start, device_entry->len, 0);
				else
					base = device_entry->start;

				driver_entry->init(base, d, device_entry->data);
			}
		}
		device_entry++;
	}

	return 0;
}

int init_timer(struct device_init_entry *hw)
{
	return timer_setup(hw);
}
