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

#ifndef _ASM_RISCV_CSR_H
#define _ASM_RISCV_CSR_H

#define RISCV_XLEN 64

#define INSERT_FIELD(val, which, fieldval) \
	(((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

#define MSTATUS_FS_SHIFT	13
#define MSTATUS_FS	(3UL << MSTATUS_FS_SHIFT)

#define MENVCFG_STCE_SHIFT  63
#define MENVCFG_STCE        (1UL << MENVCFG_STCE_SHIFT)

#define MCOUNTEREN_TM_SHIFT  1
#define MCOUNTEREN_TM       (1UL << MCOUNTEREN_TM_SHIFT)

#define MENVCFG_PBMTE_SHIFT 62
#define MENVCFG_PBMTE       (1UL << MENVCFG_PBMTE_SHIFT)

#define MENVCFG_CBIE_SHIFT   4
#define MENVCFG_CBIE         (3UL << MENVCFG_CBIE_SHIFT)

#define MENVCFG_CBCFE_SHIFT  6
#define MENVCFG_CBCFE        (1UL << MENVCFG_CBCFE_SHIFT)

#define MENVCFG_CBZE_SHIFT   7
#define MENVCFG_CBZE         (1UL << MENVCFG_CBZE_SHIFT)

#define PRV_U				(0UL)
#define PRV_S				(1UL)
#define PRV_M				(3UL)

#define MSTATUS_MPP_SHIFT	11
#define MSTATUS_MPP	(3UL << MSTATUS_MPP_SHIFT)
#define MSTATUS_MPIE	0x00000080UL

/* Status register flags */
#define SSTATUS_SPP_SHIFT	8
#define SSTATUS_SPP	(1UL << SSTATUS_SPP_SHIFT)

#define SR_SIE  0x2UL		/* Supervisor Interrupt Enable */
#define SR_SPIE 0x20UL		/* Previous Supervisor IE */
#define SR_SPP 0x100UL		/* Previously Supervisor */
#define SR_SUM	0x40000UL	/* Supervisor may access User Memory */
#define SR_FS  0x6000UL		/* Floating-point Status */
#define SR_XS  0x00018000UL	/* Extension Status */

/* Interrupt enable */
#define SIE_SSIE 0x2UL		/* IPI */
#define SIE_STIE 0x20UL		/* TIMER */
#define SIE_SEIE 0x200UL	/* EXTERN IRQ */

#define SCAUSE_INT (1UL << 63)
#define is_interrupt_fault(reg) (reg & SCAUSE_INT)

#define SCAUSE_EC (0xf)

#define SATP_MODE_39 (1UL << 63)

/* VS csr */
#define CSR_VSSTATUS 0x200
#define CSR_VSIE 0x204
#define CSR_VSTVEC 0x205
#define CSR_VSSCRATCH 0x240
#define CSR_VSEPC 0x241
#define CSR_VSCAUSE 0x242
#define CSR_VSTVAL 0x243
#define CSR_VSIP 0x244
#define CSR_VSATP 0x280

/* HS csr */
#define CSR_HSTATUS 0x600
#define HSTATUS_SPV_SHIFT	7
#define HSTATUS_SPV	(1UL << HSTATUS_SPV_SHIFT)

#define HSTATUS_SPVP_SHIFT	8
#define HSTATUS_SPVP	(1UL << HSTATUS_SPVP_SHIFT)

#define CSR_HEDELEG 0x602
#define CSR_HIDELEG 0x603
#define CSR_HIE 0x604
#define CSR_HTIMEDELTA 0x605
#define CSR_HTIMEDELTAH 0x615
#define CSR_HCOUNTEREN 0x606
#define CSR_HGEIE 0x607
#define CSR_HTVAL 0x643
#define CSR_HIP 0x644
#define CSR_HVIP 0x645
#define CSR_HTINST 0x64A
#define CSR_HGATP 0x680
#define HGATP_MODE_Sv39x4 (8UL << 60)

#define CSR_HGEIP 0xE07

/* Machine Memory Protection */
#define MAX_CSR_PMP     8

#define CSR_PMPCFG0	0x3a0
#define CSR_PMPADDR0	0x3b0
#define CSR_PMPADDR1	0x3b1
#define CSR_PMPADDR2	0x3b2
#define CSR_PMPADDR3	0x3b3
#define CSR_PMPADDR4	0x3b4
#define CSR_PMPADDR5	0x3b5
#define CSR_PMPADDR6	0x3b6
#define CSR_PMPADDR7	0x3b7

#define PMP_R	0x01UL
#define PMP_W	0x02UL
#define PMP_X	0x04UL
#define PMP_A	0x18UL
#define PMP_A_TOR 0x08UL
#define PMP_A_NA4 0x10UL
#define PMP_A_NAPOT 0x18UL
#define PMP_L	 0x80UL
#define PMP_RWX (PMP_R | PMP_W | PMP_X)
#define PMP_SHIFT 2

#define __ASM_STR(x)    #x

#define read_csr(csr)						\
({								\
	register unsigned long __v;				\
	__asm__ __volatile__ ("csrr %0, "  __ASM_STR(csr)			\
			      : "=r" (__v) :			\
			      : "memory");			\
	__v;							\
})

#define write_csr(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrw "__ASM_STR(csr)", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#define csr_set(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrs "__ASM_STR(csr)", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#define csr_clear(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrc "__ASM_STR(csr)", %0"		\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#define csr_swap(csr, val)                                      \
({                                                              \
        unsigned long __v = (unsigned long)(val);               \
        __asm__ __volatile__ ("csrrw %0, " __ASM_STR(csr) ", %1"\
                              : "=r" (__v) : "rK" (__v)         \
                              : "memory");                      \
        __v;                                                    \
})

#endif
