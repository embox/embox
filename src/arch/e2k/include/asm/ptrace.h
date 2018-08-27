/**
 * @file
 *
 * @date Mar 16, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_E2K_INCLUDE_ASM_PTRACE_H_
#define SRC_ARCH_E2K_INCLUDE_ASM_PTRACE_H_

#include <stdint.h>
#ifndef __ASSEMBLER__

struct trap_info {
	char		tc_count;
	trap_cellar_t	tcellar[MAX_TC_SIZE];
};

/*
 * Структура e2k_stack - фрагмент стека для запоминания регистров
 * в момент возникновения исключительной ситуации
 */
struct e2k_stack {
        e2k_sbr_t       sbr;     /* 21 Stack base register: top of */
                                 /*    local data (user) stack */
        e2k_usd_lo_t    usd_lo;  /* 22 Local data (user) stack */
        e2k_usd_hi_t    usd_hi;  /* 23 descriptor: base & size */
        e2k_psp_lo_t    psp_lo;  /* 24 Procedure stack pointer: */
        e2k_psp_hi_t    psp_hi;  /* 25 base & index & size */
        e2k_pcsp_lo_t   pcsp_lo; /* 26 Procedure chain stack */
        e2k_pcsp_hi_t   pcsp_hi; /* 27 pointer: base & index & size */
};

struct e2k_entry_stack {
	e2k_pshtp_t pshtp;
	e2k_wd_t wd;
	e2k_psp_lo_t psp_lo;
	e2k_psp_hi_t psp_hi;
	e2k_pcsp_lo_t pcsp_lo;
	e2k_pcsp_hi_t pcsp_hi;
};

struct pt_regs {
	uint64_t lsr, lsr1, ilcr, ilcr1;
	e2k_ctpr_t ctpr1, ctpr2, ctpr3;
	e2k_upsr_t upsr;
	int asid;
	e2k_crs_t crs;
	char longjmp;
	struct TIRs TIR;
	struct trap_info trap;
	struct e2k_stack stacks;
	struct e2k_entry_stack entry_stacks;
	e2k_aasr_t aasr;
	e2k_aau_t *aau; /* Used by user threads only */
	uint64_t saved_m_iframe;

	/* TODO E2K below for compilation only */
	uint64_t dummy [4]; /* arguments of interrupt routine*/
	union {
		struct {
			uint64_t zero;     /* R_ZERO_OFFSET     */
			uint64_t AT;       /* R_AT_OFFSET       */
			uint64_t v0;       /* R_V0_OFFSET       */
			uint64_t v1;       /* R_V1_OFFSET       */
			uint64_t a0;       /* R_A0_OFFSET       */
			uint64_t a1;       /* R_A1_OFFSET       */
			uint64_t a2;       /* R_A2_OFFSET       */
			uint64_t a3;       /* R_A3_OFFSET       */
			uint64_t t0;       /* R_T0_OFFSET       */
			uint64_t t1;       /* R_T1_OFFSET       */
			uint64_t t2;       /* R_T2_OFFSET       */
			uint64_t t3;       /* R_T3_OFFSET       */
			uint64_t t4;       /* R_T4_OFFSET       */
			uint64_t t5;       /* R_T5_OFFSET       */
			uint64_t t6;       /* R_T6_OFFSET       */
			uint64_t t7;       /* R_T7_OFFSET       */
			uint64_t s0;       /* R_S0_OFFSET       */
			uint64_t s1;       /* R_S1_OFFSET       */
			uint64_t s2;       /* R_S2_OFFSET       */
			uint64_t s3;       /* R_S3_OFFSET       */
			uint64_t s4;       /* R_S4_OFFSET       */
			uint64_t s5;       /* R_S5_OFFSET       */
			uint64_t s6;       /* R_S6_OFFSET       */
			uint64_t s7;       /* R_S7_OFFSET       */
			uint64_t t8;       /* R_T8_OFFSET       */
			uint64_t t9;       /* R_T9_OFFSET       */
			uint64_t k0;       /* R_K0_OFFSET       */
			uint64_t k1;       /* R_K1_OFFSET       */
			uint64_t gp;       /* R_GP_OFFSET       */
			uint64_t sp;       /* R_SP_OFFSET       */
			uint64_t fp;       /* R_FP_OFFSET       */
			uint64_t ra;       /* R_RA_OFFSET       */
		};
		uint64_t array [32];
	};
	uint64_t hi;           /* R_MDHI_OFFSET     */
	uint64_t lo;           /* R_MDLO_OFFSET     */
	uint64_t cp0_cause;    /* R_CAUSE_OFFSET    */
	uint64_t cp0_badvaddr; /* R_BADVADDR_OFFSET */
	uint64_t cp0_epc;
	uint64_t eimr;         /* R_EIMR_OFFSET     */
	/* kernel stack top */
	uint64_t kst;          /* R_KST_OFFSET      */
	/* new value of register k1 */
	uint64_t sk1;          /* R_SK1_OFFSET      */
};

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_E2K_INCLUDE_ASM_PTRACE_H_ */
