#ifndef _E2K_API_H_
#define _E2K_API_H_

#include <e2k_mas.h>
#include <stdint.h>

#ifndef __ASSEMBLER__

#include <config/embox/arch/e2k/arch.h>
#include <framework/mod/options.h>

#define MAX_CORES OPTION_MODULE_GET(embox__arch__e2k__arch,NUMBER,cpu_count)

#endif /* __ASSEMBLER__ */

#define E2K_GET_DSREG(reg_mnemonic) \
({ \
	register uint64_t res; \
	asm volatile ("rrd \t%%" #reg_mnemonic ", %0" \
		: "=r" (res)); \
	res; \
})

#define E2K_SET_SREG(reg_mnemonic, val) \
({ \
	asm volatile ("rws \t%0, %%" #reg_mnemonic \
			: \
			: "ri" ((uint32_t) (val))); \
})

#define E2K_SET_DSREG(reg_mnemonic, val) \
({ \
	asm volatile ("rwd \t%0, %%" #reg_mnemonic \
			: \
			: "ri" ((uint64_t) (val))); \
})

/**
 * Чтение/запись (Read/write).
 */
#define _E2K_READ(addr, type, size_letter) \
({ \
	register type res; \
	asm volatile ("ld" #size_letter "\t[%1], 0, %0" \
			: "=r" (res) \
			: "r" ((uintptr_t) (addr))); \
	res; \
})

#define _E2K_WRITE(addr, val, type, size_letter) \
({ \
	asm volatile ("st" #size_letter "\t[%0], 0, %1" \
			: \
			: "r" ((uintptr_t) (addr)), \
			"r" ((type) (val))); \
})

#define E2K_READ_B(addr)         _E2K_READ ((addr), uint8_t, b)
#define E2K_READ_H(addr)         _E2K_READ ((addr), uint16_t, h)
#define E2K_READ_W(addr)         _E2K_READ ((addr), uint32_t, w)
#define E2K_READ_D(addr)         _E2K_READ ((addr), uint64_t, d)

#define E2K_WRITE_B(addr, val)   _E2K_WRITE (addr, val, uint8_t, b)
#define E2K_WRITE_H(addr, val)   _E2K_WRITE (addr, val, uint16_t, h)
#define E2K_WRITE_W(addr, val)   _E2K_WRITE (addr, val, uint32_t, w)
#define E2K_WRITE_D(addr, val)   _E2K_WRITE (addr, val, uint64_t, d)

/*
 * Do load with specified MAS
 */
#define _E2K_READ_MAS(addr, mas, type, size_letter, chan_letter) \
({ \
	register type res; \
	asm volatile ("ld" #size_letter "," #chan_letter " \t0x0, [%1] %2, %0" \
			: "=r" (res) \
			: "r" ((uintptr_t) (addr)), \
			  "i" (mas)); \
	res; \
})

#define _E2K_WRITE_MAS(addr, val, mas, type, size_letter, chan_letter) \
({ \
	asm volatile ("st" #size_letter "," #chan_letter " \t0x0, [%0] %2, %1" \
			: \
			: "r" ((uintptr_t) (addr)), \
			  "r" ((type) (val)), \
			  "i" (mas) \
			: "memory"); \
})

#define E2K_READ_MAS_B_CH(addr, mas, chan_letter) \
		_E2K_READ_MAS ((addr), (mas), uint8_t, b, chan_letter)
#define E2K_READ_MAS_H_CH(addr, mas, chan_letter) \
		_E2K_READ_MAS ((addr), (mas), uint16_t, h, chan_letter)
#define E2K_READ_MAS_W_CH(addr, mas, chan_letter) \
		_E2K_READ_MAS ((addr), (mas), uint32_t, w, chan_letter)
#define E2K_READ_MAS_D_CH(addr, mas, chan_letter) \
		_E2K_READ_MAS ((addr), (mas), uint64_t, d, chan_letter)

#define E2K_READ_MAS_B(addr, mas)  E2K_READ_MAS_B_CH ((addr), (mas), 2)
#define E2K_READ_MAS_H(addr, mas)  E2K_READ_MAS_H_CH ((addr), (mas), 2)
#define E2K_READ_MAS_W(addr, mas)  E2K_READ_MAS_W_CH ((addr), (mas), 2)
#define E2K_READ_MAS_D(addr, mas)  E2K_READ_MAS_D_CH ((addr), (mas), 2)

#define E2K_READ_MAS_B_5(addr, mas)  E2K_READ_MAS_B_CH ((addr), (mas), 5)
#define E2K_READ_MAS_H_5(addr, mas)  E2K_READ_MAS_H_CH ((addr), (mas), 5)
#define E2K_READ_MAS_W_5(addr, mas)  E2K_READ_MAS_W_CH ((addr), (mas), 5)
#define E2K_READ_MAS_D_5(addr, mas)  E2K_READ_MAS_D_CH ((addr), (mas), 5)

#define E2K_WRITE_MAS_B(addr, val, mas)  _E2K_WRITE_MAS (addr, val, mas, uint8_t, b, 2)
#define E2K_WRITE_MAS_H(addr, val, mas)  _E2K_WRITE_MAS (addr, val, mas, uint16_t, h, 2)
#define E2K_WRITE_MAS_W(addr, val, mas)  _E2K_WRITE_MAS (addr, val, mas, uint32_t, w, 2)
#define E2K_WRITE_MAS_D(addr, val, mas)  _E2K_WRITE_MAS (addr, val, mas, uint64_t, d, 2)

/*
 * On E3S atomic operations have relaxed memory ordering:
 * _st_unlock can be reordered with subsequent loads and stores.
 *
 * Issue an explicit memory barrier if atomic operation returns a value.
 */
#if !defined CONFIG_E2K_MACHINE || \
		defined CONFIG_E2K_E3S || \
		defined CONFIG_E2K_ES2_DSP || \
		defined CONFIG_E2K_ES2_RU
/*TODO E2K isn't needed on e4c+ */
# define WMB_AFTER_ATOMIC	/* E2K_WAIT_ST_C_SAS() */ \
				".word 0x00008001\n" \
				".word 0x30000084\n"
#else
# define WMB_AFTER_ATOMIC
#endif

#define E2K_WAIT(num) \
({ \
	asm volatile ("{wait ma_c=%5, fl_c=%4, "\
			"ld_c = %3, st_c=%2, all_e=%1, all_c=%0}" \
			: \
			: "i" (((num) & 0x1)), \
			  "i" (((num) & 0x2)  >> 1), \
			  "i" (((num) & 0x4)  >> 2), \
			  "i" (((num) & 0x8)  >> 3), \
			  "i" (((num) & 0x10) >> 4), \
			  "i" (((num) & 0x20) >> 5) \
			: "memory" ); \
})

/* BUG 79245 - use .word to encode relaxed barriers */
#define E2K_WAIT_ST_C_SAS() \
({ \
	_Pragma("no_asm_inline") \
	asm volatile (".word 0x00008001\n" \
			".word 0x30000084\n" \
			::: "memory"); \
})

/* BUG 79245 - use .word to encode relaxed barriers */
#define E2K_WAIT_ST_C_SAS_LD_C_SAL() \
({ \
	_Pragma("no_asm_inline") \
	asm volatile (".word 0x00008001\n" \
			".word 0x3000018c\n" \
			::: "memory"); \
})

#define E2K_WAIT_ALL E2K_WAIT(_ma_c|_fl_c|_all_e|_all_c)

/* Wait for the load to finish before issuing
 * next memory loads/stores. */
#define E2K_RF_WAIT_LOAD(reg) \
do { \
	asm volatile ("{adds %0, 0, %%empty}" :: "r" (reg) : "memory"); \
} while (0)

/*
 * Force strict CPU ordering.
 * And yes, this is required on UP too when we're talking
 * to devices.
 *
 * For now, "wmb()" doesn't actually do anything, as all
 * Intel CPU's follow what Intel calls a *Processor Order*,
 * in which all writes are seen in the program order even
 * outside the CPU.
 *
 */
#define _sal    0x100 /* store-after-load modifier for _lt_c */
#define _sas    0x080 /* store-after-store modifier for _st_c */
#define _trap   0x040 /* stop the conveyor untill interrupt */
#define _ma_c   0x020
#define _fl_c   0x010
#define _ld_c   0x008 /* stop until all load operations complete */
#define _st_c   0x004 /* stop until store operations complete */
#define _all_e  0x002
#define _all_c  0x001

/*
 * Hardware stacks flush rules for e2k:
 *
 * 1) PSP/PCSP/PSHTP/PCSHTP reads wait for the corresponding SPILL/FILL
 * to finish (whatever the reason for SPILL/FILL is - "flushc", "flushr",
 * register file overflow, etc). "rr" must not be in the same wide
 * instruction as "flushc"/"flushr".
 *
 * 2) CWD reads wait for the chain stack SPILL/FILL to finish.
 *
 * 3) "wait all_e=1" waits for all SPILL/FILL operations (thus trap handler
 * entry also waits for it).
 *
 * 4) Memory accesses do not wait, so between "flushc"/"flushr" and memory
 * access there must be a proper waiting operation (reading of register or
 * "wait all_e");
 *
 * 5) All processors after E3M have synchronous SPILL/FILL, so we redefine
 * wait to NOP for them.
 *
 * 6) PSP/PCSP writes wait _only_ for SPILL. So if we do not know whether
 * there can be a FILL going right now then some form of wait must be
 * inserted before the write. Also writing PSHTP/PCSHTP has undefined
 * behavior in instruction set, so using it is not recommended because
 * of compatibility with future processors.
 *
 * 7) "wait ma_c=1" waits for all memory accesses including those issued
 * by SPILL/FILL opertions. It does _not_ wait for SPILL/FILL itself.
 */

#define E2K_FLUSHR \
do { \
	asm volatile ("{flushr}" ::: "memory"); \
} while (0)

#define	E2K_FLUSHC \
do { \
	asm volatile ("{nop 2} {flushc; nop 3}" ::: "memory"); \
} while (0)

#define E2K_FLUSHCPU \
do { \
	asm volatile ("{flushr; nop 2} {flushc; nop 3}" ::: "memory"); \
} while (0)

#define E2K_FLUSH_WAIT \
do { \
} while (0)


/* Add ctpr3 to clobbers to explain to lcc that this
 * GNU asm does a return. */
#define E2K_DONE \
do { \
	asm volatile ("{nop 3} {done}" ::: "ctpr3"); \
} while (0)


#ifdef	__ASSEMBLER__
/* Embox version of E2K_RETURN for using in assembler code */
#define E2K_ASM_RETURN \
	nop 5; \
	ipd 2; \
	return %ctpr3; \
	ct %ctpr3; \
	ipd 3;

#define E2K_ASM_FLUSH_CPU \
	flushr; \
	nop 2;  \
	flushc; \
	nop 3;

#endif /* __ASSEMBLER__ */

/* UPSR register bits */
#define UPSR_FE   (1UL << 0) /* Enable floating operations */
#define UPSR_IE   (1UL << 5) /* Enable interrutps */
#define UPSR_NMIE (1UL << 7) /* Enable non-maskable interrupts */

/* PSR register bits */
#define PSR_PM   (1UL << 0) /* Privileged mode */
#define PSR_IE   (1UL << 1) /* Enable interrutps */
#define PSR_NMIE (1UL << 4) /* Enable non-maskable interrupts */
#define PSR_UIE  (1UL << 5) /* Allow user to control interrupts */

/* We use this macro for Embox as default, and control IPL with UPSR */
#define PSR_ALL_IRQ_ENABLED (PSR_IE | PSR_NMIE | PSR_UIE | PSR_PM)

/* PSP.lo offsets */
#define PSP_BASE  0
#define PSP_SIZE  32
#define PSP_RW    59
/* PSP.hi offsets */
#define PSP_IND   0

/* PCSP.lo offsets */
#define PCSP_BASE 0
#define PCSP_SIZE 32
#define PCSP_RW   59
/* PCSP.hi offsets */
#define PCSP_IND  0

/* USD.lo offsets */
#define USD_BASE  0
/* USD.hi offsets */
#define USD_SIZE  32
#define USD_SIZE_MASK  0xFFFFFFFFUL

/* CR0 offsets */
#define CR0_IP 3
/* CR1 offsets */
#define CR1_WBS  33
#define CR1_USSZ 36
#define CR1_PSR  57

/* Summary size of both CR0 and CR1 */
#define SZ_OF_CR0_CR1 32

#define E2_RWAR_R_ENABLE    0x1UL
#define E2_RWAR_W_ENABLE    0x2UL
#define E2_RWAR_RW_ENABLE   (E2_RWAR_R_ENABLE | E2_RWAR_W_ENABLE)

#ifndef	__ASSEMBLER__
static inline uint32_t e2k_upsr_read(void) {
	unsigned int upsr;
	asm volatile ("rrs %%upsr, %0" : "=r"(upsr) :);
	return upsr;
}

static inline void e2k_upsr_write(uint32_t val) {
	asm volatile ("rws %0, %%upsr" : : "ri"(val));
}
#endif /* !__ASSEMBLER__ */

#endif /* _E2K_API_H_ */
