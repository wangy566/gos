#include <asm/type.h>
#include <uart.h>
#include <device.h>
#include <print.h>
#include <mm.h>
#include <irq.h>
#include <trap.h>
#include <asm/csr.h>
#include <timer.h>
#include <shell.h>
#include <cpu.h>
#include <percpu.h>
#include <task.h>
#include <devicetree.h>

extern const char logo[];

void start_gos(unsigned int hart_id, struct device_init_entry *hw)
{
	early_print_setup(hw);

	print(logo);
	print("Hello, gos!\n");

	trap_init();

	mm_init(hw);

	paging_init(hw);

	early_print_setup(hw);

	parse_dtb();

	percpu_init();

	irq_init();

	irqchip_setup(hw);

	init_timer(hw);

	device_driver_init(hw);

	bringup_secondary_cpus(hw);

	percpu_tasks_init(0);

	enable_local_irq();

	set_online_cpumask(0);
	//shell_init(NULL);
	create_task("shell_init", shell_init, NULL, 0, NULL, 0);
}
