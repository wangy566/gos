#ifndef __VCPU_INSN_H
#define __VCPU_INSN_H

#include "asm/type.h"
#include "virt.h"

#define INSN_OPCODE_MASK	0x007c
#define INSN_OPCODE_SHIFT	2
#define INSN_OPCODE_SYSTEM	28

#define INSN_MASK_WFI		0xffffffff
#define INSN_MATCH_WFI		0x10500073

#define INSN_MATCH_CSRRW	0x1073
#define INSN_MASK_CSRRW		0x707f
#define INSN_MATCH_CSRRS	0x2073
#define INSN_MASK_CSRRS		0x707f
#define INSN_MATCH_CSRRC	0x3073
#define INSN_MASK_CSRRC		0x707f
#define INSN_MATCH_CSRRWI	0x5073
#define INSN_MASK_CSRRWI	0x707f
#define INSN_MATCH_CSRRSI	0x6073
#define INSN_MASK_CSRRSI	0x707f
#define INSN_MATCH_CSRRCI	0x7073
#define INSN_MASK_CSRRCI	0x707f

#define INSN_MATCH_LB		0x3
#define INSN_MASK_LB		0x707f
#define INSN_MATCH_LH		0x1003
#define INSN_MASK_LH		0x707f
#define INSN_MATCH_LW		0x2003
#define INSN_MASK_LW		0x707f
#define INSN_MATCH_LD		0x3003
#define INSN_MASK_LD		0x707f
#define INSN_MATCH_LBU		0x4003
#define INSN_MASK_LBU		0x707f
#define INSN_MATCH_LHU		0x5003
#define INSN_MASK_LHU		0x707f
#define INSN_MATCH_LWU		0x6003
#define INSN_MASK_LWU		0x707f
#define INSN_MATCH_SB		0x23
#define INSN_MASK_SB		0x707f
#define INSN_MATCH_SH		0x1023
#define INSN_MASK_SH		0x707f
#define INSN_MATCH_SW		0x2023
#define INSN_MASK_SW		0x707f
#define INSN_MATCH_SD		0x3023
#define INSN_MASK_SD		0x707f

#define INSN_MATCH_C_LD		0x6000
#define INSN_MASK_C_LD		0xe003
#define INSN_MATCH_C_SD		0xe000
#define INSN_MASK_C_SD		0xe003
#define INSN_MATCH_C_LW		0x4000
#define INSN_MASK_C_LW		0xe003
#define INSN_MATCH_C_SW		0xc000
#define INSN_MASK_C_SW		0xe003
#define INSN_MATCH_C_LDSP	0x6002
#define INSN_MASK_C_LDSP	0xe003
#define INSN_MATCH_C_SDSP	0xe002
#define INSN_MASK_C_SDSP	0xe003
#define INSN_MATCH_C_LWSP	0x4002
#define INSN_MASK_C_LWSP	0xe003
#define INSN_MATCH_C_SWSP	0xc002
#define INSN_MASK_C_SWSP	0xe003

#define SH_RD			7
#define SH_RS1			15
#define SH_RS2			20
#define SH_RS2C			2
#define MASK_RX			0x1f

#define RV_X(x, s, n)		(((x) >> (s)) & ((1 << (n)) - 1))
#define RVC_RS1S(insn)		(8 + RV_X(insn, SH_RD, 3))
#define RVC_RS2S(insn)		(8 + RV_X(insn, SH_RS2C, 3))
#define RVC_RS2(insn)		RV_X(insn, SH_RS2C, 5)

#define INSN_16BIT_MASK 0x3

#define LOG_REGBYTES		3
#define REGBYTES		(1 << LOG_REGBYTES)

#define HTINST_COMPRESSED(htinst) (!(htinst & 0x3 == 0x3))
#define INSN_IS_16BIT(insn)     (((insn) & INSN_16BIT_MASK) != INSN_16BIT_MASK)
#define INSN_LEN(insn)          (INSN_IS_16BIT(insn) ? 2 : 4)
#define INSN_FUNCT3(insn)        ((insn >> 12) & 0x3)

#define SHIFT_RIGHT(x, y)		\
	((y) < 0 ? ((x) << -(y)) : ((x) >> (y)))

#define REG_MASK			\
	((1 << (5 + LOG_REGBYTES)) - (1 << LOG_REGBYTES))

#define REG_OFFSET(insn, pos)		\
	(SHIFT_RIGHT((insn), (pos) - LOG_REGBYTES) & REG_MASK)

#define REG_PTR(insn, pos, regs)	\
	((unsigned long *)((unsigned long)(regs) + REG_OFFSET(insn, pos)))

#define GET_RS1(insn, regs)	(*REG_PTR(insn, SH_RS1, regs))
#define GET_RS2(insn, regs)	(*REG_PTR(insn, SH_RS2, regs))
#define GET_RS1S(insn, regs)	(*REG_PTR(RVC_RS1S(insn), 0, regs))
#define GET_RS2S(insn, regs)	(*REG_PTR(RVC_RS2S(insn), 0, regs))
#define GET_RS2C(insn, regs)	(*REG_PTR(insn, SH_RS2C, regs))
#define GET_SP(regs)		(*REG_PTR(2, 0, regs))
#define SET_RD(insn, regs, val)	(*REG_PTR(insn, SH_RD, regs) = (val))
#define IMM_I(insn)		((s32)(insn) >> 20)
#define IMM_S(insn)		(((s32)(insn) >> 25 << 5) | \
				 (s32)(((insn) >> 7) & 0x1f))

int vcpu_mmio_store(struct vcpu *vcpu, unsigned long fault_addr,
		    unsigned long htinst);
int vcpu_mmio_load(struct vcpu *vcpu, unsigned long fault_addr,
		   unsigned long htinst);

#endif
