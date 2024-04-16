#include <print.h>
#include <asm/ptregs.h>
#include <asm/csr.h>
#include <asm/sbi.h>
#include <irq.h>
#include "task.h"
#include "asm/pgtable.h"

extern void do_exception_vector(void);

struct fault_info {
	int (*fn)(struct pt_regs * regs, const char *name);
	const char *name;
};

void panic()
{
	print("Kernel panic\n");
	while (1) ;
}

void show_regs(struct pt_regs *regs)
{
	print("cpu_id:%d\n", sbi_get_cpu_id());
	print("sepc: 0x%lx ra : 0x%lx sp : 0x%lx\n", regs->sepc, regs->ra,
	      regs->sp);
	print(" gp : 0x%lx tp : 0x%lx t0 : 0x%lx\n", regs->gp, regs->tp,
	      regs->t0);
	print(" t1 : 0x%lx t2 : 0x%lx t3 : 0x%lx\n", regs->t1, regs->t2,
	      regs->s0);
	print(" s1 : 0x%lx a0 : 0x%lx a1 : 0x%lx\n", regs->s1, regs->a0,
	      regs->a1);
	print(" a2 : 0x%lx a3 : 0x%lx a4 : 0x%lx\n", regs->a2, regs->a3,
	      regs->a4);
	print(" a5 : 0x%lx a6 : 0x%lx a7 : 0x%lx\n", regs->a5, regs->a6,
	      regs->a7);
	print(" s2 : 0x%lx s3 : 0x%lx s4 : 0x%lx\n", regs->s2, regs->s3,
	      regs->s4);
	print(" s5 : 0x%lx s6 : 0x%lx s7 : 0x%lx\n", regs->s5, regs->s6,
	      regs->s7);
	print(" s8 : 0x%lx s9 : 0x%lx s10: 0x%lx\n", regs->s8, regs->s9,
	      regs->s10);
	print(" s11: 0x%lx t3 : 0x%lx t4: 0x%lx\n", regs->s11, regs->t3,
	      regs->t4);
	print(" t5 : 0x%lx t6 : 0x%lx\n", regs->t5, regs->t6);
}

extern int mmu_is_on;
static void do_trap_error(struct pt_regs *regs, const char *str)
{
	print("Oops - %s\n", str);
	show_regs(regs);

	if (!mmu_is_on)
		print("sstatus:0x%lx  sbadaddr:0x%lx  scause:0x%lx\n",
		      regs->sstatus, regs->sbadaddr, regs->scause);
	else
		print
		    ("sstatus:0x%lx  sbadaddr:0x%lx sbadaddr(pa):0x%lx scause:0x%lx\n",
		     regs->sstatus, regs->sbadaddr, virt_to_phy(regs->sbadaddr),
		     regs->scause);

	panic();
}

#define DO_ERROR_INFO(name)				\
int name(struct pt_regs *regs, const char *str)				\
{									\
	do_trap_error(regs, str);	\
	return 0;            \
}

DO_ERROR_INFO(do_trap_unknown);
DO_ERROR_INFO(do_trap_insn_misaligned);
DO_ERROR_INFO(do_trap_insn_fault);
DO_ERROR_INFO(do_trap_insn_illegal);
DO_ERROR_INFO(do_trap_load_misaligned);
DO_ERROR_INFO(do_trap_load_fault);
DO_ERROR_INFO(do_trap_store_misaligned);
DO_ERROR_INFO(do_trap_store_fault);
DO_ERROR_INFO(do_trap_ecall_u);
DO_ERROR_INFO(do_trap_ecall_s);
DO_ERROR_INFO(do_trap_break);
DO_ERROR_INFO(do_page_fault);

static struct fault_info fault_info[] = {
	{ do_trap_insn_misaligned, "Instruction address misaligned" },
	{ do_trap_insn_fault, "Instruction access fault" },
	{ do_trap_insn_illegal, "Illegal instruction" },
	{ do_trap_break, "Breakpoint" },
	{ do_trap_load_misaligned, "Load address misaligned" },
	{ do_trap_load_fault, "Load access fault" },
	{ do_trap_store_misaligned, "Store/AMO address misaligned" },
	{ do_trap_store_fault, "Store/AMO access fault" },
	{ do_trap_ecall_u, "Environment call from U-mode" },
	{ do_trap_ecall_s, "Environment call from S-mode" },
	{ do_trap_unknown, "unknown 10" },
	{ do_trap_unknown, "unknown 11" },
	{ do_page_fault, "Instruction page fault" },
	{ do_page_fault, "Load page fault" },
	{ do_trap_unknown, "unknown 14" },
	{ do_page_fault, "Store/AMO page fault" },
};

static inline struct fault_info *ec_to_fault_info(unsigned int scause)
{
	return fault_info + (scause & SCAUSE_EC);
}

#define INTERRUPT_CAUSE_SOFTWARE    1
#define INTERRUPT_CAUSE_TIMER       5
#define INTERRUPT_CAUSE_EXTERNAL    9

#define EXC_INST_MISALIGNED     0
#define EXC_INST_ACCESS         1
#define EXC_BREAKPOINT          3
#define EXC_LOAD_ACCESS         5
#define EXC_STORE_ACCESS        7
#define EXC_SYSCALL             8
#define EXC_INST_PAGE_FAULT     12
#define EXC_LOAD_PAGE_FAULT     13
#define EXC_STORE_PAGE_FAULT    15

static int handle_exception(struct pt_regs *regs, unsigned long cause)
{
	struct fault_info *fi;

	switch (cause) {
	default:
		fi = ec_to_fault_info(cause);
		if (fi)
			fi->fn(regs, fi->name);
		else {
			print("unknown exception!! -- %d\n", cause);
			panic();
		}
	}

	return 0;
}

int do_exception(struct pt_regs *regs, unsigned long scause)
{
	if (scause & (1UL << 63)) {
		task_scheduler_enter(regs);
		handle_irq(scause);
		task_scheduler_exit(regs);
	} else {
		handle_exception(regs, scause);
	}

	return 0;
}

void trap_init(void)
{
	write_csr(sscratch, 0);

	write_csr(stvec, do_exception_vector);
	print("stvec=0x%x, 0x%x\n", read_csr(stvec), do_exception_vector);

	write_csr(sie, -1);
}
