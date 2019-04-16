#ifndef _E2K_API_H_
#define _E2K_API_H_

#include <e2k_mas.h>
#include <stdint.h>

#ifndef	__ASSEMBLER__

#include <config/embox/arch/e2k/arch.h>
#include <framework/mod/options.h>

#define MAX_CORES OPTION_MODULE_GET(embox__arch__e2k__arch,NUMBER,cpu_count)

typedef void *__e2k_ptr_t;

#endif /* __ASSEMBLER__ */


#define E2K_SET_REG(reg_no, val) \
({ \
	asm volatile ("adds \t0x0, %0, %%r" #reg_no \
		: \
		: "ri" ((uint32_t) (val))); \
})

#define E2K_SET_DREG(reg_no, val) \
({ \
	asm volatile ("addd \t0x0, %0, %%dr" #reg_no \
		: \
		: "ri" ((uint64_t) (val))); \
})

#define E2K_SET_DGREG(reg_no, val) \
({ \
	asm volatile ("addd \t0x0, %0, %%dg" #reg_no \
		: \
		: "ri" ((uint64_t) (val))); \
})
#define E2K_SET_DGREG_NV(reg_no, val) \
({ \
	asm ("addd \t%0, 0, %%dg" #reg_no \
		: \
		: "ri" ((uint64_t) (val))); \
})


#define E2K_GET_BREG(reg_no) \
({ \
	register uint32_t res; \
	asm volatile ("adds \t0x0, %%b[" #reg_no "], %0" \
		 : "=r" (res)); \
		res; \
})

#define E2K_GET_DBREG(reg_no) \
({ \
	register uint64_t res; \
	asm volatile ("addd \t0x0, %%db[" #reg_no "], %0" \
		: "=r" (res)); \
		res; \
})

#define E2K_SET_BREG(reg_no, val) \
({ \
	asm volatile ("adds \t0x0, %0, %%b[" #reg_no "]" \
		: \
		: "ri" ((uint32_t) (val))); \
})

#define E2K_SET_DBREG(reg_no, val) \
({ \
	asm volatile ("addd \t0x0, %0, %%db[" #reg_no "]" \
		: \
		: "ri" ((uint64_t) (val))); \
})

#define E2K_GET_SREG(reg_mnemonic) \
({ \
	register uint32_t res; \
	asm volatile ("rrs \t%%" #reg_mnemonic ", %0" \
		: "=r" (res)); \
	res; \
})

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

#define E2K_SET_CTPR(reg_mnemonic, val) \
({ \
	asm volatile ("rwd \t%0, %%" #reg_mnemonic \
			: \
			: "ri" ((uint64_t) (val)) \
			: #reg_mnemonic ); \
})


#ifndef	__ASSEMBLER__

typedef unsigned long __e2k_syscall_arg_t;

#define E2K_CALL_CLOBBERS \
		"ctpr1", "ctpr2", "ctpr3", \
		"b[0]", "b[1]", "b[2]", "b[3]", \
		"b[4]", "b[5]", "b[6]", "b[7]"
#define E2K_SYSCALL_CLOBBERS E2K_CALL_CLOBBERS

/* Transaction operation transaction of argument type
 * __e2k_syscall_arg_t */
#ifdef __ptr64__
#define __E2K_SYSCAL_ARG_ADD "addd,s"
#else
#define __E2K_SYSCAL_ARG_ADD "adds,s"
#endif

#define __E2K_SYSCALL_0(_trap, _sys_num, _arg1) \
({ \
	register __e2k_syscall_arg_t __res; \
	asm volatile ("{\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[sys_num], %%b[0]\n\t" \
		"sdisp %%ctpr1, %[trap]\n\t" \
		"}\n" \
		"call  %%ctpr1, wbs = %#\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %%b[0], %[res]" \
		: [res]     "=r" (__res) \
		: [trap]    "i"  ((int) (_trap)), \
		  [sys_num] "ri" ((__e2k_syscall_arg_t) (_sys_num)) \
		: E2K_SYSCALL_CLOBBERS); \
	__res; \
})

#define __E2K_SYSCALL_1(_trap, _sys_num, _arg1) \
({ \
	register __e2k_syscall_arg_t __res; \
	asm volatile ("{\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[sys_num], %%b[0]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg1], %%b[1]\n\t" \
		"sdisp %%ctpr1, %[trap]\n\t" \
		"}\n" \
		"call  %%ctpr1, wbs = %#\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %%b[0], %[res]" \
		: [res]     "=r" (__res) \
		: [trap]    "i"  ((int) (_trap)), \
		  [sys_num] "ri" ((__e2k_syscall_arg_t) (_sys_num)), \
		  [arg1]    "ri" ((__e2k_syscall_arg_t) (_arg1)) \
		: E2K_SYSCALL_CLOBBERS); \
	__res; \
})

#define __E2K_SYSCALL_2(_trap, _sys_num, _arg1, _arg2) \
({ \
	register __e2k_syscall_arg_t __res; \
	asm volatile ("{\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[sys_num], %%b[0]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg1], %%b[1]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg2], %%b[2]\n\t" \
		"sdisp %%ctpr1, %[trap]\n\t" \
		"}\n" \
		"call  %%ctpr1, wbs = %#\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %%b[0], %[res]" \
		: [res]     "=r" (__res) \
		: [trap]    "i"  ((int) (_trap)), \
		  [sys_num] "ri" ((__e2k_syscall_arg_t) (_sys_num)), \
		  [arg1]    "ri" ((__e2k_syscall_arg_t) (_arg1)), \
		  [arg2]    "ri" ((__e2k_syscall_arg_t) (_arg2)) \
		: E2K_SYSCALL_CLOBBERS); \
	__res; \
})

#define __E2K_SYSCALL_3(_trap, _sys_num, _arg1, _arg2, _arg3) \
({ \
	register __e2k_syscall_arg_t __res; \
	asm volatile ("{\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[sys_num], %%b[0]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg1], %%b[1]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg2], %%b[2]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg3], %%b[3]\n\t" \
		"sdisp %%ctpr1, %[trap]\n\t" \
		"}\n" \
		"call  %%ctpr1, wbs = %#\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %%b[0], %[res]" \
		: [res]     "=r" (__res) \
		: [trap]    "i"  ((int) (_trap)), \
		  [sys_num] "ri" ((__e2k_syscall_arg_t) (_sys_num)), \
		  [arg1]    "ri" ((__e2k_syscall_arg_t) (_arg1)), \
		  [arg2]    "ri" ((__e2k_syscall_arg_t) (_arg2)), \
		  [arg3]    "ri" ((__e2k_syscall_arg_t) (_arg3)) \
		: E2K_SYSCALL_CLOBBERS); \
	__res; \
})

#define __E2K_SYSCALL_4(_trap, _sys_num, _arg1, _arg2, _arg3, _arg4) \
({ \
	register __e2k_syscall_arg_t __res; \
	asm volatile ("{\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[sys_num], %%b[0]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg1], %%b[1]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg2], %%b[2]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg3], %%b[3]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg4], %%b[4]\n\t" \
		"sdisp %%ctpr1, %[trap]\n\t" \
		"}\n" \
		"call  %%ctpr1, wbs = %#\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %%b[0], %[res]" \
		: [res]     "=r" (__res) \
		: [trap]    "i"  ((int) (_trap)), \
		 [sys_num] "ri" ((__e2k_syscall_arg_t) (_sys_num)), \
		 [arg1]    "ri" ((__e2k_syscall_arg_t) (_arg1)), \
		 [arg2]    "ri" ((__e2k_syscall_arg_t) (_arg2)), \
		 [arg3]    "ri" ((__e2k_syscall_arg_t) (_arg3)), \
		 [arg4]    "ri" ((__e2k_syscall_arg_t) (_arg4)) \
		: E2K_SYSCALL_CLOBBERS); \
	__res; \
})

#define __E2K_SYSCALL_5(_trap, _sys_num, _arg1, _arg2, _arg3, _arg4, _arg5) \
({ \
	register __e2k_syscall_arg_t __res; \
	asm volatile ("{\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[sys_num], %%b[0]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg1], %%b[1]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg2], %%b[2]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg3], %%b[3]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg4], %%b[4]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg5], %%b[5]\n\t" \
		"sdisp %%ctpr1, %[trap]\n\t" \
		"}\n" \
		"call  %%ctpr1, wbs = %#\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %%b[0], %[res]" \
		: [res]     "=r" (__res) \
		: [trap]    "i"  ((int) (_trap)), \
		  [sys_num] "ri" ((__e2k_syscall_arg_t) (_sys_num)), \
		  [arg1]    "ri" ((__e2k_syscall_arg_t) (_arg1)), \
		  [arg2]    "ri" ((__e2k_syscall_arg_t) (_arg2)), \
		  [arg3]    "ri" ((__e2k_syscall_arg_t) (_arg3)), \
		  [arg4]    "ri" ((__e2k_syscall_arg_t) (_arg4)), \
		  [arg5]    "ri" ((__e2k_syscall_arg_t) (_arg5)) \
		: E2K_SYSCALL_CLOBBERS); \
	__res; \
})

#define __E2K_SYSCALL_6(_trap, _sys_num, _arg1, \
			_arg2, _arg3, _arg4, _arg5, _arg6) \
({ \
	register __e2k_syscall_arg_t __res; \
	asm volatile ("{\n" \
		"sdisp %%ctpr1, %[trap]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[sys_num], %%b[0]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg1], %%b[1]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg2], %%b[2]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg3], %%b[3]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg4], %%b[4]\n\t" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg5], %%b[5]\n\t" \
		"}\n" \
		"{\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %[arg6], %%b[6]\n\t" \
		"call  %%ctpr1, wbs = %#\n\t" \
		"}\n" \
		__E2K_SYSCAL_ARG_ADD "  0x0, %%b[0], %[res]" \
		: [res]     "=r" (__res) \
		: [trap]    "i"  ((int) (_trap)), \
		  [sys_num] "ri" ((__e2k_syscall_arg_t) (_sys_num)), \
		  [arg1]    "ri" ((__e2k_syscall_arg_t) (_arg1)), \
		  [arg2]    "ri" ((__e2k_syscall_arg_t) (_arg2)), \
		  [arg3]    "ri" ((__e2k_syscall_arg_t) (_arg3)), \
		  [arg4]    "ri" ((__e2k_syscall_arg_t) (_arg4)), \
		  [arg5]    "ri" ((__e2k_syscall_arg_t) (_arg5)), \
		  [arg6]    "ri" ((__e2k_syscall_arg_t) (_arg6)) \
		: E2K_SYSCALL_CLOBBERS); \
	__res; \
})

#define E2K_SYSCALL(trap, sys_num, num_args, args...) \
	__E2K_SYSCALL_##num_args(trap, sys_num, args)

#endif /* !__ASSEMBLER__ */



/* External tags. */

#define	ETAGNUM		0x00		/* Num. value. generic */

#define	ETAGNVS		0x00		/* Num. value single */
#define	ETAGEWS		0x01		/* Empty value. single */
#define	ETAGDWS		0x01		/* Diagnostic value. single */

#define	ETAGNVD		0x00		/* Num. value double */
#define	ETAGNPD		0x00		/* Null pointer */
#define	ETAGEWD		0x05		/* Empty value. double */
#define	ETAGDWD		0x05		/* Diagnostic value. double */
#define	ETAGPLD		0x0A		/* Procedure label */

#define	ETAGNVQ		0x00		/* Num. value quadro */
#define	ETAGNPQ		0x00		/* Null pointer */
#define	ETAGAPQ		0xCF		/* Array pointer */
#define	ETAGSAP		0xCF		/* Stack array pointer */

#define	ETAGBADQ	0xee	/* Invalid tag for quadro object */


/* 
 * It needs to exclude optimitazion in loop
 * when kernel coped user's date and may be incorrect user addresses
 * In this case interrupt really do longjump to TRAP_RETURN_LABEL!!
 * This define must be used in any loop with user addresses
 */ 
#define	E2K_CMD_SEPARATOR		asm volatile ("{nop}" ::: "memory")

/* To avoid header dependencies use this define
 * instead of BUILD_BUG_ON() from <hal/kernel.h>. */
#define E2K_BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#ifndef E2K_BIN_VER
#define E2K_BIN_VER	8
#endif

#define	EI_SEMANTIC	7

#define	ELF_CODE_UNKNOWN                0
#define	ELF_CODE_32_UNPROTECTED         1
#define	ELF_CODE_64_UNPROTECTED         2
#define	ELF_CODE_NEW_PROTECTED          5
#define ELF_CODE_NEW_PROTECTED_CXX      6

#define ELF_BIN_COMP		0x4
/* 
 * This flag is used only for EM_E2K cpu_type
 *  If  x->e_flags && ELF_E2K_INCOMPAT == 1 
 *  the code can executed only (mtype==0) -     any
 *                                   ==1        e3m 
 *                                   ==2        e3s
 *                                   ==3        e2s
 *                                   ==4        e8c
 */                             
#define ELF_E2K_INCOMPAT        0x10 

#define IS_INCOMPAT(x)		((x)->e_machine == EM_E2K && \
				((x)->e_flags & ELF_E2K_INCOMPAT))

/* protected mode flag */
#define ELF_E2K_PM	        0x20
/* Does it have E2S hardware bug workaround? */
#define ELF_E2K_BUG_67911	0x40

#define EM_E2KL 	(33 + (E2K_BIN_VER << 1))     /* Little endian */
#define EM_E2KB 	(34 + (E2K_BIN_VER << 1))     /* Big endian */
#define EM_E2K_FAKE	EM_E2KL
#define EM_E2K		175

/* Machine type checker. Is to be used for 64-bit, 32-bit elf
 * and protected mode. Result depends on machine type (mtype: e3m or e3s)
 * and binary type(apptype: e3m or e3s). Implements the following check:
 *  
 * ========================================================================
 * | mtype/apptype | app_e3m | app_e3s | app_e2s | app_e8c |app_e2k (all) |
 * ------------------------------------------------------------------------
 * |      e3m      |    +    |    -    |    -    |    -    |      +	  |
 * ------------------------------------------------------------------------
 * |      e3s      |    -    |    +    |    -    |    -    |	  +	  |
 * ------------------------------------------------------------------------
 * |	  e2s	   |    -    |    +    |    +	 |    -	   |	  +	  |
 * ------------------------------------------------------------------------
 * |      e8c      |    -    |    +    |    +	 |    +	   |	  +	  |
 * ========================================================================
 */

#define elf_check_e2k_mtype(x)						\
({									\
	unsigned long mt;						\
	int _res  = 0;							\
	int _iset = pMACHINE->iset_ver;					\
	int _rev  = pMACHINE->rev;					\
									\
	if ((x)->e_machine == EM_E2K) {				\
		mt = (unsigned long)((x)->e_flags >> 24);		\
	} else {							\
		mt = (unsigned long)((x)->e_flags >> 28) & 0x7;		\
	}								\
									\
	switch (mt) {							\
	case 0:								\
		if (!IS_INCOMPAT(x) || _iset == ELBRUS_ISET		\
				|| _iset == ELBRUS_S_ISET)		\
			_res = 1;					\
		break;							\
	case 1:								\
		if (_iset == ELBRUS_ISET)				\
			_res = 1;					\
		break;							\
	case 2:								\
		if (!IS_INCOMPAT(x) && _iset > ELBRUS_S_ISET		\
				|| _iset == ELBRUS_S_ISET)		\
			_res = 1;					\
		break;							\
	case 3:								\
		if (!IS_INCOMPAT(x) && _iset > ELBRUS_2S_ISET		\
				|| _iset == ELBRUS_2S_ISET)		\
			_res = 1;					\
		break;							\
	case 4:								\
		if (!IS_INCOMPAT(x) && _iset > ELBRUS_8C_ISET		\
				|| _iset == ELBRUS_8C_ISET)		\
			_res = 1;					\
		break;							\
	case 5:								\
		if (!IS_INCOMPAT(x) && _iset > ELBRUS_8C2_ISET		\
				|| _iset == ELBRUS_8C2_ISET)		\
			_res = 1;					\
		break;							\
	case 16:							\
		if (!IS_INCOMPAT(x) && _iset >= ELBRUS_S_ISET		\
				|| _iset == ELBRUS_S_ISET && _rev == 0)	\
			_res = 1;					\
		break;							\
	case 17:							\
		if (!IS_INCOMPAT(x) && (_iset > ELBRUS_S_ISET		\
				|| _iset == ELBRUS_S_ISET && _rev > 1)	\
				|| _iset == ELBRUS_S_ISET && _rev == 1)	\
			_res = 1;					\
		break;							\
	case 18:							\
		if (!IS_INCOMPAT(x) && _iset > ELBRUS_S_ISET		\
				|| _iset == ELBRUS_S_ISET && _rev == 2)	\
			_res = 1;					\
		break;							\
	default:							\
		break;							\
	}								\
									\
	_res;								\
})


#define	LDST_BYTE_FMT		1UL	/* load/store byte (8 bits) */
#define	LDST_HALF_FMT		2UL	/* load/store halfword (16 bits) */
#define	LDST_WORD_FMT		3UL	/* load/store word (32 bits) */
#define	LDST_DWORD_FMT		4UL	/* load/store double-word (64 bits) */
#define	LDST_QWORD_FMT		5UL	/* load/store quad-word (128 bits) */
#define	LDST_QP_FMT		7UL	/* load/store qpacked word (128 bits) */

#define	LDST_REC_OPC_INDEX_SHIFT	 0
#define	LDST_REC_OPC_INDEX_SIZE		32	/* [31- 0] byte index */
#define	LDST_REC_OPC_MAS_SHIFT		32
#define	LDST_REC_OPC_MAS_SIZE		 7	/* [38-32] MAS */
#define	LDST_REC_OPC_PROT_SHIFT		39
#define	LDST_REC_OPC_PROT_SIZE		 1	/*    [39] protected access */
#define	LDST_REC_OPC_FMT_SHIFT		40
#define	LDST_REC_OPC_FMT_SIZE		 3	/* [42-40] format of access */
#define	LDST_REC_OPC_ROOT_SHIFT		43
#define	LDST_REC_OPC_ROOT_SIZE		 1	/*    [43] virtual space */
						/*	   type flag */
#define	LDST_REC_OPC_RG_SHIFT		44
#define	LDST_REC_OPC_RG_SIZE		 8	/* [51-44] physical address */
						/*	   of an NR (in terms */
						/*	   of single-NR) used */
						/*	   for handling */
						/*	   memory locks */
						/*	   conflicts */
#define	LDST_REC_OPC_UNUSED_SHIFT	52
#define	LDST_REC_OPC_UNUZED_SIZE	4	/* [55-52] unused bits */

#define LDST_REC_OPC_BYPASS_L1		(MAS_BYPASS_L1_CACHE << \
							LDST_REC_OPC_MAS_SHIFT)
#define LDST_REC_OPC_BYPASS_CACHE	(MAS_BYPASS_ALL_CACHES << \
							LDST_REC_OPC_MAS_SHIFT)

#define	TAGGED_MEM_LOAD_REC_OPC ( \
			LDST_QWORD_FMT << LDST_REC_OPC_FMT_SHIFT | \
			MAS_FILL_OPERATION << LDST_REC_OPC_MAS_SHIFT)
#define	TAGGED_MEM_STORE_REC_OPC (LDST_QWORD_FMT << LDST_REC_OPC_FMT_SHIFT)

/*
 * Normal simulator termination
 */
#define E2K_LMS_HALT_OK				\
({						\
	_Pragma("no_asm_inline")		\
	asm volatile (".word \t0x00008001\n\t"	\
			".word \t0x60000000");	\
})

/*
 * Simulator termination on error
 */
#define E2K_LMS_HALT_ERROR(err_no)		\
({						\
	_Pragma("no_asm_inline")		\
	asm volatile (".word \t0x00008001\n\t"	\
		".word \t0x60000000 | %0"	\
		:				\
		: "i" (err_no));		\
})

/*
 * Kprobes breakpoint instruction
 */
#define E2K_KPROBES_BREAKPOINT		\
({					\
	_Pragma("no_asm_inline")		\
	asm volatile (".word \t0x04000001\n\t"	\
			".word \t0x0dc0c040");	\
})

#define E2K_GET_REG(reg_no) \
({ \
    register uint32_t res; \
    asm volatile ("adds \t0x0, %%r" #reg_no ", %0"  \
                  : "=r" (res)); \
    res; \
})

#define E2K_GET_DREG(reg_no) \
({ \
    register uint64_t res; \
    asm volatile ("addd \t0x0, %%dr" #reg_no ", %0"  \
                  : "=r" (res)); \
    res; \
})

#define E2K_GET_DGREG(reg_no) \
({ \
    register uint64_t res; \
    asm volatile ("addd \t0x0, %%dg" #reg_no ", %0"  \
                  : "=r" (res)); \
    res; \
})


#define __E2K_QPSWITCHD_SM_GREG(num)				\
do {								\
	asm ("qpswitchd,sm %%dg" #num ", %%dg" #num		\
			::: "%g" #num);				\
} while (0)

#define E2K_QPSWITCHD_SM_GREG(greg_num)					\
do {									\
	switch (greg_num) {						\
	case  0: __E2K_QPSWITCHD_SM_GREG(0); break;			\
	case  1: __E2K_QPSWITCHD_SM_GREG(1); break;			\
	case  2: __E2K_QPSWITCHD_SM_GREG(2); break;			\
	case  3: __E2K_QPSWITCHD_SM_GREG(3); break;			\
	case  4: __E2K_QPSWITCHD_SM_GREG(4); break;			\
	case  5: __E2K_QPSWITCHD_SM_GREG(5); break;			\
	case  6: __E2K_QPSWITCHD_SM_GREG(6); break;			\
	case  7: __E2K_QPSWITCHD_SM_GREG(7); break;			\
	case  8: __E2K_QPSWITCHD_SM_GREG(8); break;			\
	case  9: __E2K_QPSWITCHD_SM_GREG(9); break;			\
	case 10: __E2K_QPSWITCHD_SM_GREG(10); break;			\
	case 11: __E2K_QPSWITCHD_SM_GREG(11); break;			\
	case 12: __E2K_QPSWITCHD_SM_GREG(12); break;			\
	case 13: __E2K_QPSWITCHD_SM_GREG(13); break;			\
	case 14: __E2K_QPSWITCHD_SM_GREG(14); break;			\
	case 15: __E2K_QPSWITCHD_SM_GREG(15); break;			\
	case 16: __E2K_QPSWITCHD_SM_GREG(16); break;			\
	case 17: __E2K_QPSWITCHD_SM_GREG(17); break;			\
	case 18: __E2K_QPSWITCHD_SM_GREG(18); break;			\
	case 19: __E2K_QPSWITCHD_SM_GREG(19); break;			\
	case 20: __E2K_QPSWITCHD_SM_GREG(20); break;			\
	case 21: __E2K_QPSWITCHD_SM_GREG(21); break;			\
	case 22: __E2K_QPSWITCHD_SM_GREG(22); break;			\
	case 23: __E2K_QPSWITCHD_SM_GREG(23); break;			\
	case 24: __E2K_QPSWITCHD_SM_GREG(24); break;			\
	case 25: __E2K_QPSWITCHD_SM_GREG(25); break;			\
	case 26: __E2K_QPSWITCHD_SM_GREG(26); break;			\
	case 27: __E2K_QPSWITCHD_SM_GREG(27); break;			\
	case 28: __E2K_QPSWITCHD_SM_GREG(28); break;			\
	case 29: __E2K_QPSWITCHD_SM_GREG(29); break;			\
	case 30: __E2K_QPSWITCHD_SM_GREG(30); break;			\
	case 31: __E2K_QPSWITCHD_SM_GREG(31); break;			\
	default: assert(0);						\
	}								\
} while (0)

/*
 * Copy single register tagged value to single register
 * src_reg - local value of type single register to copy from
 * dst_reg - local value of type single register to copy to
 */
#define E2K_MOVE_TAGGED_REG(src_reg, dst_reg) \
({ \
	asm volatile ("movts \t%1, %0"  \
			: "=r" (dst_reg) \
			: "r" ((uint32_t) (src_reg))); \
})

#define _E2K_GET_DGREG_VAL_AND_TAG(greg_no, dst_reg, tag)	\
({								\
	u_int32_t __dtag;						\
	asm volatile ("{gettagd %%dg" #greg_no ", %0\n\t"	\
		      "puttagd %%dg" #greg_no ", 0, %1}"	\
		      : "=r" (__dtag), "=r" (dst_reg)		\
		      : );					\
	tag = __dtag;						\
})

#define E2K_GET_DGREG_VAL_AND_TAG(greg_num, dst_reg, tag)	     \
({								     \
	switch (greg_num) {					     \
	case  0: _E2K_GET_DGREG_VAL_AND_TAG(0, dst_reg, tag); break;  \
	case  1: _E2K_GET_DGREG_VAL_AND_TAG(1, dst_reg, tag); break;  \
	case  2: _E2K_GET_DGREG_VAL_AND_TAG(2, dst_reg, tag); break;  \
	case  3: _E2K_GET_DGREG_VAL_AND_TAG(3, dst_reg, tag); break;  \
	case  4: _E2K_GET_DGREG_VAL_AND_TAG(4, dst_reg, tag); break;  \
	case  5: _E2K_GET_DGREG_VAL_AND_TAG(5, dst_reg, tag); break;  \
	case  6: _E2K_GET_DGREG_VAL_AND_TAG(6, dst_reg, tag); break;  \
	case  7: _E2K_GET_DGREG_VAL_AND_TAG(7, dst_reg, tag); break;  \
	case  8: _E2K_GET_DGREG_VAL_AND_TAG(8, dst_reg, tag); break;  \
	case  9: _E2K_GET_DGREG_VAL_AND_TAG(9, dst_reg, tag); break;  \
	case 10: _E2K_GET_DGREG_VAL_AND_TAG(10, dst_reg, tag); break; \
	case 11: _E2K_GET_DGREG_VAL_AND_TAG(11, dst_reg, tag); break; \
	case 12: _E2K_GET_DGREG_VAL_AND_TAG(12, dst_reg, tag); break; \
	case 13: _E2K_GET_DGREG_VAL_AND_TAG(13, dst_reg, tag); break; \
	case 14: _E2K_GET_DGREG_VAL_AND_TAG(14, dst_reg, tag); break; \
	case 15: _E2K_GET_DGREG_VAL_AND_TAG(15, dst_reg, tag); break; \
	case 16: _E2K_GET_DGREG_VAL_AND_TAG(16, dst_reg, tag); break; \
	case 17: _E2K_GET_DGREG_VAL_AND_TAG(17, dst_reg, tag); break; \
	case 18: _E2K_GET_DGREG_VAL_AND_TAG(18, dst_reg, tag); break; \
	case 19: _E2K_GET_DGREG_VAL_AND_TAG(19, dst_reg, tag); break; \
	case 20: _E2K_GET_DGREG_VAL_AND_TAG(20, dst_reg, tag); break; \
	case 21: _E2K_GET_DGREG_VAL_AND_TAG(21, dst_reg, tag); break; \
	case 22: _E2K_GET_DGREG_VAL_AND_TAG(22, dst_reg, tag); break; \
	case 23: _E2K_GET_DGREG_VAL_AND_TAG(23, dst_reg, tag); break; \
	case 24: _E2K_GET_DGREG_VAL_AND_TAG(24, dst_reg, tag); break; \
	case 25: _E2K_GET_DGREG_VAL_AND_TAG(25, dst_reg, tag); break; \
	case 26: _E2K_GET_DGREG_VAL_AND_TAG(26, dst_reg, tag); break; \
	case 27: _E2K_GET_DGREG_VAL_AND_TAG(27, dst_reg, tag); break; \
	case 28: _E2K_GET_DGREG_VAL_AND_TAG(28, dst_reg, tag); break; \
	case 29: _E2K_GET_DGREG_VAL_AND_TAG(29, dst_reg, tag); break; \
	case 30: _E2K_GET_DGREG_VAL_AND_TAG(30, dst_reg, tag); break; \
	case 31: _E2K_GET_DGREG_VAL_AND_TAG(31, dst_reg, tag); break; \
	default: assert(0);					      \
	}							      \
})

#define _E2K_SET_DGREG_VAL_AND_TAG(greg_no, val, tag)		\
do {								\
	asm volatile ("puttagd %0, %1, %%dg" #greg_no		\
		      :						\
		      : "r" (val), "r" (tag));			\
} while (0)

#define E2K_SET_DGREG_VAL_AND_TAG(greg_num, val, tag)	     	  \
do {								  \
	switch (greg_num) {					  \
	case  0: _E2K_SET_DGREG_VAL_AND_TAG(0, val, tag); break;  \
	case  1: _E2K_SET_DGREG_VAL_AND_TAG(1, val, tag); break;  \
	case  2: _E2K_SET_DGREG_VAL_AND_TAG(2, val, tag); break;  \
	case  3: _E2K_SET_DGREG_VAL_AND_TAG(3, val, tag); break;  \
	case  4: _E2K_SET_DGREG_VAL_AND_TAG(4, val, tag); break;  \
	case  5: _E2K_SET_DGREG_VAL_AND_TAG(5, val, tag); break;  \
	case  6: _E2K_SET_DGREG_VAL_AND_TAG(6, val, tag); break;  \
	case  7: _E2K_SET_DGREG_VAL_AND_TAG(7, val, tag); break;  \
	case  8: _E2K_SET_DGREG_VAL_AND_TAG(8, val, tag); break;  \
	case  9: _E2K_SET_DGREG_VAL_AND_TAG(9, val, tag); break;  \
	case 10: _E2K_SET_DGREG_VAL_AND_TAG(10, val, tag); break; \
	case 11: _E2K_SET_DGREG_VAL_AND_TAG(11, val, tag); break; \
	case 12: _E2K_SET_DGREG_VAL_AND_TAG(12, val, tag); break; \
	case 13: _E2K_SET_DGREG_VAL_AND_TAG(13, val, tag); break; \
	case 14: _E2K_SET_DGREG_VAL_AND_TAG(14, val, tag); break; \
	case 15: _E2K_SET_DGREG_VAL_AND_TAG(15, val, tag); break; \
	case 16: _E2K_SET_DGREG_VAL_AND_TAG(16, val, tag); break; \
	case 17: _E2K_SET_DGREG_VAL_AND_TAG(17, val, tag); break; \
	case 18: _E2K_SET_DGREG_VAL_AND_TAG(18, val, tag); break; \
	case 19: _E2K_SET_DGREG_VAL_AND_TAG(19, val, tag); break; \
	case 20: _E2K_SET_DGREG_VAL_AND_TAG(20, val, tag); break; \
	case 21: _E2K_SET_DGREG_VAL_AND_TAG(21, val, tag); break; \
	case 22: _E2K_SET_DGREG_VAL_AND_TAG(22, val, tag); break; \
	case 23: _E2K_SET_DGREG_VAL_AND_TAG(23, val, tag); break; \
	case 24: _E2K_SET_DGREG_VAL_AND_TAG(24, val, tag); break; \
	case 25: _E2K_SET_DGREG_VAL_AND_TAG(25, val, tag); break; \
	case 26: _E2K_SET_DGREG_VAL_AND_TAG(26, val, tag); break; \
	case 27: _E2K_SET_DGREG_VAL_AND_TAG(27, val, tag); break; \
	case 28: _E2K_SET_DGREG_VAL_AND_TAG(28, val, tag); break; \
	case 29: _E2K_SET_DGREG_VAL_AND_TAG(29, val, tag); break; \
	case 30: _E2K_SET_DGREG_VAL_AND_TAG(30, val, tag); break; \
	case 31: _E2K_SET_DGREG_VAL_AND_TAG(31, val, tag); break; \
	default: assert(0);				          \
	}							  \
} while (0)


#if __iset__ == 2
#define E2K_SAVE_GREG(__addr_lo, __addr_hi, numlo, numhi, iset)		\
({									\
	u_int64_t reg0, reg1;							\
	BUILD_BUG_ON(iset != E2K_ISET_V1);				\
									\
	asm (								\
		"strd,2 [ %[addr_lo] + %[opc_0] ], %%dg" #numlo "\n"	\
		"strd,5 [ %[addr_hi] + %[opc_0] ], %%dg" #numhi "\n"	\
		"movfi %%dg" #numlo ", %[reg0]\n"			\
		"movfi %%dg" #numhi ", %[reg1]\n"			\
		"sth [ %[addr_lo] + 8 ], %[reg0]\n"			\
		"sth [ %[addr_hi] + 8 ], %[reg1]\n"			\
		: [reg0] "=&r" (reg0), [reg1] "=&r" (reg1)		\
		: [addr_lo] "r" (__addr_lo), [addr_hi] "r" (__addr_hi),	\
		  [opc_0] "i" (TAGGED_MEM_STORE_REC_OPC)		\
		: "memory");						\
})

#define E2K_RESTORE_GREG(__addr_lo, __addr_hi, numlo, numhi, iset)	\
({									\
	u_int64_t reg0, reg1, reg2, reg3;					\
	BUILD_BUG_ON(iset != E2K_ISET_V1);				\
									\
	asm (								\
		"ldrd,2 [ %[addr_lo] + %[opc_0] ], %%dg" #numlo "\n"	\
		"ldrd,5 [ %[addr_hi] + %[opc_0] ], %%dg" #numhi "\n"	\
		"ldh [ %[addr_lo] + 8 ], %[reg0]\n"			\
		"ldh [ %[addr_hi] + 8 ], %[reg1]\n"			\
		"gettagd %%dg" #numlo ", %[reg2]\n"			\
		"gettagd %%dg" #numhi ", %[reg3]\n"			\
		"cmpesb 0, %[reg2], %%pred2\n"				\
		"cmpesb 0, %[reg3], %%pred3\n"				\
		"movif %%dg" #numlo ", %[reg0], %%dg" #numlo " ? %%pred2\n" \
		"movif %%dg" #numhi ", %[reg1], %%dg" #numhi " ? %%pred3\n" \
		: [reg0] "=&r" (reg0),	[reg1] "=&r" (reg1),		\
		  [reg2] "=&r" (reg2),	[reg3] "=&r" (reg3)		\
		: [addr_lo] "r" (__addr_lo), [addr_hi] "r" (__addr_hi),	\
		  [opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC)			\
		:"%g" #numlo, "%g" #numhi, "%pred2", "%pred3");		\
})

#elif __iset__ == 5

#define E2K_SAVE_GREG(__addr_lo, __addr_hi, numlo, numhi, iset)		\
({									\
	u_int64_t reg0, reg1;							\
	BUILD_BUG_ON(iset != E2K_ISET_V5);				\
									\
	asm (								\
		"stqp,2,sm [ %[addr_lo] + 0 ], %%dg" #numlo "\n"	\
		"stqp,5,sm [ %[addr_hi] + 0 ], %%dg" #numhi "\n"	\
		"strd,2 [ %[addr_lo] + %[opc_0] ], %%dg" #numlo "\n"	\
		"strd,5 [ %[addr_hi] + %[opc_0] ], %%dg" #numhi "\n"	\
		: [reg0] "=&r" (reg0), [reg1] "=&r" (reg1)		\
		: [addr_lo] "r" (__addr_lo), [addr_hi] "r" (__addr_hi),	\
		  [opc_0] "i" (TAGGED_MEM_STORE_REC_OPC)		\
		: "memory");						\
})

#define E2K_RESTORE_GREG(__addr_lo, __addr_hi, numlo, numhi, iset)	\
({									\
	u_int64_t reg0, reg1;							\
	BUILD_BUG_ON(iset != E2K_ISET_V5);				\
									\
	asm (								\
		"ldrd,2 [ %[addr_lo] + %[opc_0] ], %%dg" #numlo "\n"	\
		"ldrd,5 [ %[addr_hi] + %[opc_0] ], %%dg" #numhi "\n"	\
		"gettagd %%dg" #numlo ", %[reg0]\n"			\
		"gettagd %%dg" #numhi ", %[reg1]\n"			\
		"cmpbsb 5, %[reg0], %%pred2\n"				\
		"cmpbsb 5, %[reg1], %%pred3\n"				\
		"ldqp,2 [ %[addr_lo] + 0 ], %%dg" #numlo " ? ~ %%pred2\n" \
		"ldqp,5 [ %[addr_hi] + 0 ], %%dg" #numhi " ? ~ %%pred3\n" \
		: [reg0] "=&r" (reg0),	[reg1] "=&r" (reg1)		\
		: [addr_lo] "r" (__addr_lo), [addr_hi] "r" (__addr_hi),	\
		  [opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC)			\
		: "%g" #numlo, "%g" #numhi, "%pred2", "%pred3");	\
})

#else
/* IMPORTANT: Do NOT use these macros directly, use
 * pMACHINE->save_gregs()/pMACHINE->restore_gregs() instead */
#endif

#define E2K_RESTORE_GREG_IN_SYSCALL(base, numlo1, numhi1, numlo2, numhi2) \
do {									\
	asm (								\
		"ldrd,2 [%0 + %1], %%dg" #numlo1 "\n"			\
		"ldrd,5 [%0 + %2], %%dg" #numhi1 "\n"			\
		"ldrd,2 [%0 + %3], %%dg" #numlo2 "\n"			\
		"ldrd,5 [%0 + %4], %%dg" #numhi2 "\n"			\
		:							\
		: "r" (base),						\
		  "i" (TAGGED_MEM_LOAD_REC_OPC),			\
		  "i" (TAGGED_MEM_LOAD_REC_OPC | 16UL),			\
		  "i" (TAGGED_MEM_LOAD_REC_OPC | 32UL),			\
		  "i" (TAGGED_MEM_LOAD_REC_OPC | 48UL)			\
		: "%g" #numlo1, "%g" #numhi1, "%g" #numlo2, "%g" #numhi2); \
} while (0)


#define E2K_GREGS_SET_EMPTY()					\
({								\
	register char tag;					\
	tag = ETAGEWD;						\
	asm ("puttagd 0, %0, %%dg0\n"				\
	     "puttagd 0, %0, %%dg1\n"				\
	     "puttagd 0, %0, %%dg2\n"				\
	     "puttagd 0, %0, %%dg3\n"				\
	     "puttagd 0, %0, %%dg4\n"				\
	     "puttagd 0, %0, %%dg5\n"				\
	     "puttagd 0, %0, %%dg6\n"				\
	     "puttagd 0, %0, %%dg7\n"				\
	     "puttagd 0, %0, %%dg8\n"				\
	     "puttagd 0, %0, %%dg9\n"				\
	     "puttagd 0, %0, %%dg10\n"				\
	     "puttagd 0, %0, %%dg11\n"				\
	     "puttagd 0, %0, %%dg12\n"				\
	     "puttagd 0, %0, %%dg13\n"				\
	     "puttagd 0, %0, %%dg14\n"				\
	     "puttagd 0, %0, %%dg15\n"				\
	     /* g16-g19 are used by kernel */			\
	     /*"puttagd 0, %0, %%dg16\n"*/			\
	     /*"puttagd 0, %0, %%dg17\n"*/			\
	     /*"puttagd 0, %0, %%dg18\n"*/			\
	     /*"puttagd 0, %0, %%dg19\n"*/			\
	     "puttagd 0, %0, %%dg20\n"				\
	     "puttagd 0, %0, %%dg21\n"				\
	     "puttagd 0, %0, %%dg22\n"				\
	     "puttagd 0, %0, %%dg23\n"				\
	     "puttagd 0, %0, %%dg24\n"				\
	     "puttagd 0, %0, %%dg25\n"				\
	     "puttagd 0, %0, %%dg26\n"				\
	     "puttagd 0, %0, %%dg27\n"				\
	     "puttagd 0, %0, %%dg28\n"				\
	     "puttagd 0, %0, %%dg29\n"				\
	     "puttagd 0, %0, %%dg30\n"				\
	     "puttagd 0, %0, %%dg31\n"				\
	     :							\
	     : "ri" ((char) (tag))				\
	     : "%g0", "%g1", "%g2", "%g3", "%g4", "%g5",	\
	       "%g6", "%g7", "%g8", "%g9", "%g10", "%g11",	\
	       "%g12", "%g13", "%g14", "%g15", /*"%g16",*/	\
	       /*"%g17", "%g18", "%g19",*/ "%g20", "%g21",	\
	       "%g22", "%g23", "%g24", "%g25", "%g26",		\
	       "%g27", "%g28", "%g29", "%g30", "%g31");		\
})

/*
 * We copy the value,tag and extension for all global regs
 * (we must copy all componets of register with bad tags too)
 */
#define	E2K_GET_GREGS_FROM_THREAD(_g_u, _gt_u, _base)			\
({									\
	u_int64_t reg0, reg1, reg2, reg3, reg6, reg7, reg8;			\
									\
	asm (								\
		"addd %[base], 0x0, %[r6]\n"				\
		"addd 0, 0x0, %[r7]\n"					\
		"addd 0, 0x0, %[r8]\n"					\
									\
		"1:\n"							\
		"ldrd,2 [%[r6] + %[opc_0]], %[val_lo]\n"		\
		"ldrd,5 [%[r6] + %[opc_16]], %[val_hi]\n"		\
		"addd %[r6], 32, %[r6]\n"				\
									\
		"gettagd,2 %[val_lo], %[tag_lo]\n"			\
		"gettagd,5 %[val_hi], %[tag_hi]\n"			\
		"shls %[tag_hi], 8, %[tag_hi]\n"			\
		"ors %[tag_lo], %[tag_hi], %[tag_lo]\n"			\
		"sth [%[gt_u], %[r8]], %[tag_lo]\n"			\
		"addd %[r8], 2, %[r8]\n"				\
									\
		"puttagd,2 %[val_lo], 0, %[val_lo]\n"			\
		"std [%[g_u], %[r7]], %[val_lo]\n"			\
		"addd %[r7], 8, %[r7]\n"				\
									\
		"puttagd,5 %[val_hi], 0, %[val_hi]\n"			\
		"std [%[g_u], %[r7]], %[val_hi]\n"			\
		"addd %[r7], 8, %[r7]\n"				\
									\
		"disp %%ctpr3, 1b\n"					\
		"cmpedb %[r8], 32, %%pred2\n"				\
		"ct %%ctpr3 ? ~ %%pred2\n"				\
									\
		: [val_lo] "=&r"(reg0), [val_hi] "=&r"(reg1),		\
		  [tag_lo] "=&r"(reg2), [tag_hi] "=&r"(reg3),		\
		  [r6] "=&r"(reg6), [r7] "=&r"(reg7), [r8] "=&r"(reg8)	\
		: [g_u] "r"(_g_u), [gt_u] "r"(_gt_u), [base] "r"(_base),\
		  [opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [opc_16] "i" (TAGGED_MEM_LOAD_REC_OPC | 16UL)		\
		: "%ctpr3", "%pred1", "%pred2", "memory");		\
})

#define	E2K_SET_GREGS_TO_THREAD(_base, _g_u, _gt_u)			\
({									\
	u_int64_t reg0, reg1, reg2, reg3, reg6, reg7, reg8;			\
									\
	asm (								\
		"addd 0, 0x0, %[r6]\n"					\
		"addd 0, 0x0, %[r7]\n"					\
		"addd %[base], 0x0, %[r8]\n"				\
									\
		"2:\n"							\
		"ldd [%[g_u], %[r6]], %[val_lo]\n"			\
		"addd %[r6], 8, %[r6]\n"				\
		"ldd [%[g_u], %[r6]], %[val_hi]\n"			\
		"addd %[r6], 8, %[r6]\n"				\
									\
		"ldb [%[gt_u], %[r7]], %[tag_lo]\n"			\
		"addd %[r7], 1, %[r7]\n"				\
		"ldb [%[gt_u], %[r7]], %[tag_hi]\n"			\
		"addd %[r7], 1, %[r7]\n"				\
									\
		"puttagd,2 %[val_lo], %[tag_lo], %[val_lo]\n"		\
		"puttagd,5 %[val_hi], %[tag_hi], %[val_hi]\n"		\
									\
		"strd,2 [%[r8] + %[opc_0]], %[val_lo]\n"		\
		"strd,5 [%[r8] + %[opc_16]], %[val_hi]\n"		\
		"addd %[r8], 32, %[r8]\n"				\
									\
		"disp %%ctpr3, 2b\n"					\
									\
		"cmpedb %[r7], 32, %%pred2\n"				\
		"ct %%ctpr3 ? ~ %%pred2\n"				\
									\
		: [val_lo] "=&r"(reg0), [val_hi] "=&r"(reg1),		\
		  [tag_lo] "=&r"(reg2), [tag_hi] "=&r"(reg3),		\
		  [r6] "=&r"(reg6), [r7] "=&r"(reg7), [r8] "=&r"(reg8)	\
		: [base] "r"(_base), [g_u] "r"(_g_u), [gt_u] "r"(_gt_u),\
		  [opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [opc_16] "i" (TAGGED_MEM_STORE_REC_OPC | 16UL)	\
		: "%ctpr3", "%pred2", "memory");			\
})

#define CONFIG_OPTIMIZE_REGISTERS_ACCESS

/* Allow for lcc optimizations of registers reads and writes */
#if defined CONFIG_OPTIMIZE_REGISTERS_ACCESS && \
		defined __LCC__ && __LCC__ >= 120
# define E2K_GET_SREG_NV(reg_mnemonic) \
({ \
	register uint32_t res; \
	asm ("rrs %%" #reg_mnemonic ", %0" \
		: "=r" (res)); \
	res; \
})

# define E2K_GET_DSREG_NV(reg_mnemonic) \
({ \
	register uint64_t res; \
	asm ("rrd %%" #reg_mnemonic ", %0" \
		: "=r" (res)); \
	res; \
})

# define E2K_SET_SREG_NV(reg_mnemonic, val) \
({ \
	/* Fake return value is needed for lcc to optimize inline asm... */ \
	register uint32_t res; \
	asm ("rws %1, %%" #reg_mnemonic \
		: "=r" (res) \
		: "ri" ((uint32_t) (val))); \
})

# define E2K_SET_DSREG_NV(reg_mnemonic, val) \
({ \
	/* Fake return value is needed for lcc to optimize inline asm... */ \
	register uint64_t res; \
	asm ("rwd %1, %%" #reg_mnemonic \
		: "=r" (res) \
		: "ri" ((uint64_t) (val))); \
})

/*
 * *_NOIRQ version is for psp.hi/pcsp.hi/cr/cutd
 *
 * Rules for writing:
 * 1) There must be NO exceptions and interrupts
 * 2) As a consequence of 1), instructions that are placed _later_
 * than "rw" should not generate exceptions too because compiler
 * can reorder them before the "rw" instruction.
 *
 * IOW in the whole area covered by arch_irq_save()/arch_irq_restore()
 * there must not be any exception-generating instructions.
 */

# define E2K_SET_DSREG_NV_NOIRQ(reg_mnemonic, val) \
({ \
	register uint64_t res; \
	asm ("rwd %1, %%" #reg_mnemonic \
		: "=r" (res) \
		: "ri" ((uint64_t) (val))); \
})

#else
# define E2K_GET_SREG_NV	E2K_GET_SREG
# define E2K_GET_DSREG_NV	E2K_GET_DSREG
# define E2K_SET_SREG_NV	E2K_SET_SREG
# define E2K_SET_DSREG_NV	E2K_SET_DSREG
# define E2K_SET_DSREG_NV_NOIRQ	E2K_SET_DSREG
#endif


#define E2K_EXIT_SIMPLE_SYSCALL(sbr, usd_hi, usd_lo, upsr) \
({ \
	asm volatile ("{rws \t%3, %%upsr}" \
		      "{rwd \t%2, %%usd.lo}" \
		      "{rwd \t%1, %%usd.hi}" \
		      "{rwd \t%0, %%sbr}" \
		      "{nop 2}" \
		      : \
		      : "ri" ((uint64_t) (sbr)), \
			"ri" ((uint64_t) (usd_hi)), \
			"ri" ((uint64_t) (usd_lo)), \
			"ri" ((uint32_t) (upsr))); \
})


#define E2K_SET_SREG_CLOSED(reg_mnemonic, val) \
({ \
	asm volatile ("{rws \t%0, %%" #reg_mnemonic "}" \
		      : \
		      : "ri" ((uint32_t) (val))); \
})

#define E2K_SET_DSREG_CLOSED(reg_mnemonic, val) \
({ \
	asm volatile ("{rwd \t%0, %%" #reg_mnemonic "}" \
		      : \
		      : "ri" ((uint64_t) (val))); \
})

/* lcc ignores manually specified clobbers for opened GNU asm,
 * so use closed version (bug #69565, bug #60599) */
#define E2K_SET_PSR_IRQ_BARRIER(val) \
({ \
	asm volatile ("{\n" \
		      "nop 5\n" \
		      "rws %0, %%psr" \
		      "}" \
		      : \
		      : "ri" ((uint64_t) (val)) \
		      : IRQ_BARRIER_CLOBBERS); \
})
#define E2K_SET_UPSR_IRQ_BARRIER(val) \
({ \
	asm volatile ("{\n" \
		      "nop 4\n" \
		      "rws %0, %%upsr" \
		      "}" \
		      : \
		      : "ri" ((uint64_t) (val)) \
		      : IRQ_BARRIER_CLOBBERS); \
})
#define E2K_SET_UPSR_IRQ_BARRIER_NO_NOPS(val) \
({ \
	asm volatile ("{\n" \
		      "rws %0, %%upsr" \
		      "}" \
		      : \
		      : "ri" ((uint64_t) (val)) \
		      : IRQ_BARRIER_CLOBBERS); \
})


#define E2K_GET_MMUREG_2(reg_mnemonic) \
({ \
    register uint64_t res; \
    asm volatile ("mmurr,2 \t%%" #reg_mnemonic ", %0" \
                  : "=r" (res)); \
    res; \
})

#define E2K_GET_MMUREG(reg_mnemonic) \
({ \
    register uint64_t res; \
    asm volatile ("mmurr,5 \t%%" #reg_mnemonic ", %0" \
                  : "=r" (res)); \
    res; \
})

#define E2K_SET_MMUREG(reg_mnemonic, val) \
({ \
    asm volatile ("mmurw,2 \t%0, %%" #reg_mnemonic \
                  : \
                  : "r" ((uint64_t) (val))); \
})


#define E2K_GET_AAUREG(reg_mnemonic, chan_letter) \
({ \
    register uint32_t res; \
    asm ("aaurr," #chan_letter " \t%%" #reg_mnemonic ", %0" \
                  : "=r" (res)); \
    res; \
})

/* This macro is used to pack two 'aaurr' into one long instruction */
#define E2K_GET_AAUREGS(l_reg, r_reg, lval, rval) \
({ \
    asm ("aaurr,2 \t%%" #l_reg ", %0\n" \
         "aaurr,5 \t%%" #r_reg ", %1" \
         : "=r" (lval), "=r" (rval)); \
})

#define E2K_SET_AAUREG(reg_mnemonic, val, chan_letter) \
({ \
    asm volatile ("aaurw," #chan_letter " \t%0, %%" #reg_mnemonic \
                  : \
                  : "r" ((uint32_t) (val))); \
})

/* This macro is used to pack two 'aaurr' into one long instruction */
#define E2K_SET_AAUREGS(l_reg, r_reg, lval, rval) \
({ \
    asm ("aaurw,2 \t%0, %%" #l_reg "\n" \
         "aaurw,5 \t%1, %%" #r_reg \
         : \
         : "r" ((uint32_t) (lval)), "r" ((uint32_t) (rval))); \
})

#define E2K_GET_AAUDREG(reg_mnemonic, chan_letter) \
({ \
    register uint64_t res; \
    asm ("aaurrd," #chan_letter " %%" #reg_mnemonic ", %0" \
                  : "=r" (res)); \
    res; \
})

#define E2K_GET_AAUDREGS(l_reg, r_reg, lval, rval) \
({ \
    asm ("aaurrd,2 %%" #l_reg ", %0\n" \
         "aaurrd,5 %%" #r_reg ", %1" \
         : "=r" (lval), "=r" (rval)); \
})


#define E2K_SET_AAUDREG(reg_mnemonic, val) \
({ \
    asm volatile ("aaurwd \t%0, %%" #reg_mnemonic \
                  : \
                  : "r" ((uint64_t) (val))); \
})

#define E2K_SET_AAUDREGS(l_reg, r_reg, lval, rval) \
({ \
    asm ("aaurwd,2 %0, %%" #l_reg "\n" \
         "aaurwd,5 %1, %%" #r_reg \
         : \
         : "r" (lval), "r" (rval)); \
})


#define E2K_GET_AAUQREG(mem_p, reg_mnemonic) \
({ \
    register uint64_t lo asm ("%b[0]"); \
    register uint64_t hi asm ("%b[1]"); \
    asm volatile ("aaurrq \t%%" #reg_mnemonic ", %%qb[0]" \
                  : \
                  : \
                  : "%b[0]", "%b[1]"); \
    (mem_p)->lo = lo; \
    (mem_p)->hi = hi; \
})

#define E2K_GET_AAUQREGS(mem_p, reg1, reg2, reg3, reg4) \
({ \
    register uint64_t lo1 asm ("%b[0]"); \
    register uint64_t hi1 asm ("%b[1]"); \
    register uint64_t lo2 asm ("%b[2]"); \
    register uint64_t hi2 asm ("%b[3]"); \
    register uint64_t lo3 asm ("%b[4]"); \
    register uint64_t hi3 asm ("%b[5]"); \
    register uint64_t lo4 asm ("%b[6]"); \
    register uint64_t hi4 asm ("%b[7]"); \
    asm volatile ("aaurrq \t%%" #reg1 ", %%qb[0]\n" \
                  "aaurrq \t%%" #reg2 ", %%qb[2]\n" \
                 "aaurrq \t%%" #reg3 ", %%qb[4]\n" \
                 "aaurrq \t%%" #reg4 ", %%qb[6]\n" \
                  : \
                  : \
                  : "%b[0]", "%b[1]", "%b[2]", "%b[3]", \
                   "%b[4]", "%b[5]", "%b[6]", "%b[7]"); \
    (mem_p)->lo = lo1; \
    (mem_p)->hi = hi1; \
    (mem_p + 1)->lo = lo2; \
    (mem_p + 1)->hi = hi2; \
    (mem_p + 2)->lo = lo3; \
    (mem_p + 2)->hi = hi3; \
    (mem_p + 3)->lo = lo4; \
    (mem_p + 3)->hi = hi4; \
})

#define E2K_SET_AAUQREG(reg_mnemonic, mem_p) \
({ \
    register uint64_t lo asm ("%b[0]"); \
    register uint64_t hi asm ("%b[1]"); \
    lo = (mem_p)->lo; \
    hi = (mem_p)->hi; \
    asm volatile ("aaurwq \t%%qb[0], %%" #reg_mnemonic \
                  : \
                  : "r" (lo), "r" (hi) \
                  : "%b[0]", "%b[1]"); \
})

#define E2K_SET_AAUQREGS(mem_p, reg1, reg2, reg3, reg4) \
({ \
    asm volatile ("{\n" \
                  "ldd,0 [ %0 + 0x0 ], %%db[0]\n" \
                  "ldd,2 [ %0 + 0x8 ], %%db[1]\n" \
                  "ldd,3 [ %0 + 0x10 ], %%db[2]\n" \
                  "ldd,5 [ %0 + 0x18 ], %%db[3]\n" \
                  "}\n" \
                  "{\n" \
                  "ldd,0 [ %0 + 0x20 ], %%db[4]\n" \
                  "ldd,2 [ %0 + 0x28 ], %%db[5]\n" \
                  "ldd,3 [ %0 + 0x30 ], %%db[6]\n" \
                  "ldd,5 [ %0 + 0x38 ], %%db[7]\n" \
                  "}\n" \
                  "aaurwq \t%%qb[0], %%" #reg1 "\n" \
                  "aaurwq \t%%qb[2], %%" #reg2 "\n" \
                  "aaurwq \t%%qb[4], %%" #reg3 "\n" \
                  "aaurwq \t%%qb[6], %%" #reg4 "\n" \
                  : \
                  : "r" (mem_p) \
                  : "%b[0]", "%b[1]", "%b[2]", "%b[3]", \
                   "%b[4]", "%b[5]", "%b[6]", "%b[7]"); \
})

/* Clear AAU to prepare it for restoring */
#define E2K_CLEAR_APB() \
({ \
	asm volatile ("1:\n" \
		      "{ipd 0; disp %%ctpr2, 1b}" \
		      : \
		      : \
		      : "ctpr2"); \
})

/* Stop AAU if it was active.
 * 'wait all_e = 1' is needed to allow reading of MMU registers
 * (see "Scheduling 1.2.2" for details). */
#define E2K_AAU_EAP() \
({ \
       asm volatile ("{eap; nop 2}\n" \
                     "{wait all_e = 1}\n"); \
})

/**
 * Чтение/запись (Read/write).
 */
#define _E2K_READ(addr, type, size_letter) \
({ \
    register type res; \
    asm volatile ("ld" #size_letter "\t[%1], 0, %0" \
                  : "=r" (res) \
                  : "r" ((__e2k_ptr_t) (addr))); \
    res; \
})

#define _E2K_READ_FAST(addr, type, size_letter) \
({ \
    register type res; \
    asm ("ld" #size_letter "\t[%1], %0" \
         : "=r" (res) \
         : "m" (*(unsigned long long *) (addr))); \
    res; \
})

#define _E2K_WRITE(addr, val, type, size_letter) \
({ \
    asm volatile ("st" #size_letter "\t[%0], 0, %1" \
                  : \
                  : "r" ((__e2k_ptr_t) (addr)), \
                    "r" ((type) (val))); \
})

#define E2K_READ_B(addr)  _E2K_READ ((addr), uint8_t, b)
#define E2K_READ_H(addr)  _E2K_READ ((addr), uint16_t, h)
#define E2K_READ_W(addr)  _E2K_READ ((addr), uint32_t, w)
#define E2K_READ_D(addr)  _E2K_READ ((addr), uint64_t, d)

#define E2K_READ_B_FAST(addr)  _E2K_READ_FAST ((addr), uint8_t, b)
#define E2K_READ_H_FAST(addr)  _E2K_READ_FAST ((addr), uint16_t, h)
#define E2K_READ_W_FAST(addr)  _E2K_READ_FAST ((addr), uint32_t, w)
#define E2K_READ_D_FAST(addr)  _E2K_READ_FAST ((addr), uint64_t, d)

#define E2K_WRITE_B(addr, val)  _E2K_WRITE (addr, val, uint8_t, b)
#define E2K_WRITE_H(addr, val)  _E2K_WRITE (addr, val, uint16_t, h)
#define E2K_WRITE_W(addr, val)  _E2K_WRITE (addr, val, uint32_t, w)
#define E2K_WRITE_D(addr, val)  _E2K_WRITE (addr, val, uint64_t, d)

/*
 * Do load with specified MAS
 */
#define _E2K_READ_MAS(addr, mas, type, size_letter, chan_letter) \
({ \
    register type res; \
    asm volatile ("ld" #size_letter "," #chan_letter " \t0x0, [%1] %2, %0" \
                  : "=r" (res) \
                  : "r" ((__e2k_ptr_t) (addr)), \
                    "i" (mas)); \
    res; \
})

#define _E2K_WRITE_MAS(addr, val, mas, type, size_letter, chan_letter) \
({ \
    asm volatile ("st" #size_letter "," #chan_letter " \t0x0, [%0] %2, %1" \
                  : \
                  : "r" ((__e2k_ptr_t) (addr)), \
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
 * Read from and write to system configuration registers SIC
 * Now SIC is the same as NBSRs registers
 */

#define E2K_SET_SICREG(reg_mnemonic, val, cln, pln) \
({ \
	register uint64_t addr; \
	register uint64_t node_id = (cln) << 2; \
	node_id = node_id + ((pln)&0x3); \
	addr = (uint64_t) THE_NODE_NBSR_PHYS_BASE(node_id); \
	addr = addr + SIC_##reg_mnemonic; \
	E2K_WRITE_MAS_W(addr,val, MAS_IOADDR); \
})
#define E2K_GET_SICREG(reg_mnemonic, cln, pln) \
({ \
	register uint32_t res; \
	register uint64_t addr; \
	register uint64_t node_id = (cln) << 2; \
	node_id = node_id + ((pln)&0x3); \
	addr = (uint64_t) THE_NODE_NBSR_PHYS_BASE(node_id); \
	addr = addr + SIC_##reg_mnemonic; \
	res = E2K_READ_MAS_W(addr, MAS_IOADDR); \
	res; \
})


/*
 * Prefetching with fully speculative load is
 * needed when the passed address can be invalid.
 */
#if defined __LCC__ && __LCC__ >= 116
# define E2K_PREFETCH_L2_SPEC(addr) \
({ \
	asm ("ldb,sm \t[%0] %1, 0x0, %%empty" \
		: \
		: "r" ((__e2k_ptr_t) (addr)), \
		  "i" (MAS_LOAD_SPEC | MAS_BYPASS_L1_CACHE)); \
})
#else
# define E2K_PREFETCH_L2_SPEC(addr) ({do {} while (0);})
#endif


/**
 * Операции по восстановлению (Recovery operations).
 */
#define E2K_RECOVERY_LOAD_TO(addr, opc, val, chan_letter) \
({ \
 	asm volatile ("ldrd," #chan_letter "\t[%1 + %2], %0" \
 			: "=r"(val) \
 			: "r" ((__e2k_ptr_t) (addr)), \
 			  "r" ((uint64_t) (opc))); \
})

#define E2K_RECOVERY_LOAD_TO_CH(addr, opc, val, trap_cellar_chan) \
do { \
	switch (trap_cellar_chan) { \
	case 0: \
		E2K_RECOVERY_LOAD_TO(addr, opc, val, 0); \
		break; \
	case 1: \
		E2K_RECOVERY_LOAD_TO(addr, opc, val, 2); \
		break; \
	case 2: \
		E2K_RECOVERY_LOAD_TO(addr, opc, val, 3); \
		break; \
	case 3: \
		E2K_RECOVERY_LOAD_TO(addr, opc, val, 5); \
		break; \
	} \
} while (0)

#define E2K_RECOVERY_QUADRO_LOAD(addr, opc_lo, opc_hi) \
({ \
	asm volatile ("{ldrd,0\t[%0 + %1], %%b[0]\n" \
			"ldrd,2\t[%0 + %2], %%b[1]}" \
			: \
			: "r" ((__e2k_ptr_t) (addr)), \
			"r" ((uint64_t) (opc_lo)) \
			"r" ((uint64_t) (opc_hi)) \
			: "b[0]", "b[1]"); \
})

#define E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, greg_no, chan_letter) \
({ \
	asm volatile ("ldrd," #chan_letter "\t[%0 + %1], %%dg" #greg_no \
			: \
			: "r" ((__e2k_ptr_t) (addr)), \
			  "r" ((uint64_t) (opc))); \
})

#define E2K_LOAD_TAGGED_DGREGS(addr, numlo, numhi)	\
do {							\
	asm ("ldrd,2 [%0 + %1], %%dg" #numlo "\n"	\
	     "ldrd,5 [%0 + %2], %%dg" #numhi "\n"	\
	     :						\
	     : "r" (addr),				\
	       "i" (TAGGED_MEM_LOAD_REC_OPC),		\
	       "i" (TAGGED_MEM_LOAD_REC_OPC | 8UL)	\
	     : "%g" #numlo, "%g" #numhi);		\
} while (0)

#define E2K_STORE_TAGGED_DGREG(addr, greg_no) \
do { \
	asm ("strd [%0 + %1], %%dg" #greg_no \
	     : \
	     : "r" (addr), "i" (TAGGED_MEM_STORE_REC_OPC)); \
} while (0)

/*
 * The same as E2K_RECOVERY_LOAD_TO_THE_GREG but does not touch
 * the right 32-bits word.
 */
#define E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, greg_no, chan_letter) \
({							\
	register u_int64_t __tmp;				\
	asm ("ldrd," #chan_letter" [%1 + %2], %0\n"	\
	     "movts %%g" #greg_no ", %0\n"		\
	     "movtd %0, %%dg" #greg_no "\n"		\
	     : "=r" (__tmp)				\
	     : "r" (addr), "r" (opc));			\
})


/*
 * Recovery (load) from memory value with tag to the static global register
 * greg_num - local value of dinamic # of global register to recovery
 * addr - address of value to recovery
 */
#define E2K_RECOVERY_LOAD_TO_A_GREG(addr, opc, greg_num, chan_letter) \
do { \
	switch (greg_num) { \
	case  0: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 0, chan_letter); \
		break; \
	case  1: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 1, chan_letter); \
		break; \
	case  2: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 2, chan_letter); \
		break; \
	case  3: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 3, chan_letter); \
		break; \
	case  4: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 4, chan_letter); \
		break; \
	case  5: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 5, chan_letter); \
		break; \
	case  6: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 6, chan_letter); \
		break; \
	case  7: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 7, chan_letter); \
		break; \
	case  8: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 8, chan_letter); \
		break; \
	case  9: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 9, chan_letter); \
		break; \
	case 10: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 10, chan_letter); \
		break; \
	case 11: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 11, chan_letter); \
		break; \
	case 12: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 12, chan_letter); \
		break; \
	case 13: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 13, chan_letter); \
		break; \
	case 14: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 14, chan_letter); \
		break; \
	case 15: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 15, chan_letter); \
		break; \
	case 16: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 16, chan_letter); \
		break; \
	case 17: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 17, chan_letter); \
		break; \
	case 18: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 18, chan_letter); \
		break; \
	case 19: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 19, chan_letter); \
		break; \
	case 20: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 20, chan_letter); \
		break; \
	case 21: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 21, chan_letter); \
		break; \
	case 22: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 22, chan_letter); \
		break; \
	case 23: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 23, chan_letter); \
		break; \
	case 24: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 24, chan_letter); \
		break; \
	case 25: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 25, chan_letter); \
		break; \
	case 26: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 26, chan_letter); \
		break; \
	case 27: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 27, chan_letter); \
		break; \
	case 28: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 28, chan_letter); \
		break; \
	case 29: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 29, chan_letter); \
		break; \
	case 30: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 30, chan_letter); \
		break; \
	case 31: \
		E2K_RECOVERY_LOAD_TO_THE_GREG(addr, opc, 31, chan_letter); \
		break; \
	default: \
		assert(0); \
	} \
} while (0)

#define E2K_RECOVERY_LOAD_TO_A_GREG_CH(addr, opc, greg_num, trap_cellar_chan) \
do { \
	switch (trap_cellar_chan) { \
	case 0: \
		E2K_RECOVERY_LOAD_TO_A_GREG(addr, opc, greg_num, 0); \
		break; \
	case 1: \
		E2K_RECOVERY_LOAD_TO_A_GREG(addr, opc, greg_num, 2); \
		break; \
	case 2: \
		E2K_RECOVERY_LOAD_TO_A_GREG(addr, opc, greg_num, 3); \
		break; \
	case 3: \
		E2K_RECOVERY_LOAD_TO_A_GREG(addr, opc, greg_num, 5); \
		break; \
	} \
} while (0)

#define E2K_RECOVERY_LOAD_TO_A_GREG_VL(addr, opc, greg_num, chan_letter) \
do { \
	switch (greg_num) { \
	case  0: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 0, chan_letter); \
		break; \
	case  1: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 1, chan_letter); \
		break; \
	case  2: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 2, chan_letter); \
		break; \
	case  3: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 3, chan_letter); \
		break; \
	case  4: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 4, chan_letter); \
		break; \
	case  5: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 5, chan_letter); \
		break; \
	case  6: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 6, chan_letter); \
		break; \
	case  7: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 7, chan_letter); \
		break; \
	case  8: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 8, chan_letter); \
		break; \
	case  9: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 9, chan_letter); \
		break; \
	case 10: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 10, chan_letter); \
		break; \
	case 11: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 11, chan_letter); \
		break; \
	case 12: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 12, chan_letter);\
		break; \
	case 13: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 13, chan_letter);\
		break; \
	case 14: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 14, chan_letter);\
		break; \
	case 15: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 15, chan_letter);\
		break; \
	case 16: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 16, chan_letter);\
		break; \
	case 17: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 17, chan_letter);\
		break; \
	case 18: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 18, chan_letter);\
		break; \
	case 19: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 19, chan_letter);\
		break; \
	case 20: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 20, chan_letter);\
		break; \
	case 21: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 21, chan_letter);\
		break; \
	case 22: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 22, chan_letter);\
		break; \
	case 23: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 23, chan_letter);\
		break; \
	case 24: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 24, chan_letter);\
		break; \
	case 25: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 25, chan_letter);\
		break; \
	case 26: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 26, chan_letter);\
		break; \
	case 27: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 27, chan_letter);\
		break; \
	case 28: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 28, chan_letter);\
		break; \
	case 29: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 29, chan_letter);\
		break; \
	case 30: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 30, chan_letter);\
		break; \
	case 31: \
		E2K_RECOVERY_LOAD_TO_THE_GREG_VL(addr, opc, 31, chan_letter);\
		break; \
	default: \
		assert(0); \
	} \
} while (0)

#define E2K_RECOVERY_LOAD_TO_A_GREG_VL_CH(addr, opc, greg_num, \
					  trap_cellar_chan) \
do { \
	switch (trap_cellar_chan) { \
	case 0: \
		E2K_RECOVERY_LOAD_TO_A_GREG_VL(addr, opc, greg_num, 0); \
		break; \
	case 1: \
		E2K_RECOVERY_LOAD_TO_A_GREG_VL(addr, opc, greg_num, 2); \
		break; \
	case 2: \
		E2K_RECOVERY_LOAD_TO_A_GREG_VL(addr, opc, greg_num, 3); \
		break; \
	case 3: \
		E2K_RECOVERY_LOAD_TO_A_GREG_VL(addr, opc, greg_num, 5); \
		break; \
	} \
} while (0)


/* strd in {} because compiler change channel */
#define E2K_RECOVERY_STORE(addr, val, opc, chan_letter)		\
({								\
	u_int64_t __tmp = val;					\
	asm volatile ("strd," #chan_letter "\t[%0 + %1], %2"	\
		    :						\
		    : "r" ((__e2k_ptr_t) (addr)),		\
		      "r" ((uint64_t) (opc)),		\
		      "r" ((uint64_t) (__tmp)));		\
})


/*
 * Load from memory & store to memory double-word value with tag
 * This value can be:
 *	two words with tags
 *	one double-word with tag
 *	any one half of quad-word with tag
 * Operands and result of macros should have types:
 *	addr	__e2k_ptr_t
 *	val	uint64_t
 *	lo	uint64_t (low half of quad-word value)
 *	hi	uint64_t (high half of quad-word value)
 */

#define E2K_LOAD_TAGGED_VALUE(addr, val) \
		E2K_RECOVERY_LOAD_TO(addr, TAGGED_MEM_LOAD_REC_OPC, val, 0)
#define E2K_STORE_TAGGED_VALUE(addr, val) \
		E2K_RECOVERY_STORE(addr, val, TAGGED_MEM_STORE_REC_OPC, 2)

#define	E2K_LOAD_TAGGED_DWORD		E2K_LOAD_TAGGED_VALUE
#define	E2K_LOAD_TAGGED_QWORD(addr, lo, hi)	\
		{E2K_LOAD_TAGGED_VALUE(addr, lo); \
		E2K_LOAD_TAGGED_VALUE(((uint64_t *)addr) + 1, hi);}


#define E2K_STORE_TAGGED_DWORD E2K_STORE_TAGGED_VALUE

/* 
 * #58441  - work with taged value (compiler problem)
 * store tag and store taged word must be in common asm code
 *  (cloused asm code)
 */ 
#define	E2K_STORE_VALUE_WITH_TAG(addr, val, tag)                        \
        E2K_STORE_TAGGED_WORD(addr, val, tag,\
                                        TAGGED_MEM_STORE_REC_OPC, 2)

#define	E2K_STORE_TAGGED_WORD(addr, val, tag, opc, chan_letter) \
do { \
	u_int64_t __tmp_reg = val; \
	E2K_BUILD_BUG_ON(sizeof(val) != 8); \
	asm volatile ("{puttagd \t%0, %2, %0\n}" \
		      " strd," #chan_letter " \t[%1 + %3], %0\n" \
		      : "+r" (__tmp_reg) \
		      : "r" ((__e2k_ptr_t) (addr)), \
			"ri" ((uint32_t) (tag)), \
			"ri" ((opc)) \
		      : "memory"); \
} while (0)

#define	E2K_STORE_TAGGED_WORD_CH(addr, val, tag, opc, trap_cellar_chan) \
do { \
	switch (trap_cellar_chan) { \
	case 1: \
		E2K_STORE_TAGGED_WORD(addr, val, tag, opc, 2); \
		break; \
	case 3: \
		E2K_STORE_TAGGED_WORD(addr, val, tag, opc, 5); \
		break; \
	} \
} while (0)


#define	E2K_STORE_TAGGED_QWORD(addr, val_lo, val_hi, tag_lo, tag_hi)    \
({                                                                      \
	u_int64_t reg1, reg2;							\
	E2K_BUILD_BUG_ON(sizeof(val_hi) != 8);                          \
	E2K_BUILD_BUG_ON(sizeof(val_lo) != 8);                          \
	asm volatile (	"{puttagd %3, %5, %0\n"                         \
			" puttagd %4, %6, %1}\n"                        \
			"{strd,2 [%2 + %7], %0\n"                       \
			" strd,5 [%2 + %8], %1}\n"                      \
			: "=&r" (reg1), "=&r" (reg2)                    \
			: "r" (addr),                                   \
			  "r" (val_lo),                                 \
			  "r" (val_hi),                                 \
			  "ri" (tag_lo),                                \
			  "ri" (tag_hi),                                \
			  "i" (TAGGED_MEM_STORE_REC_OPC),               \
			  "i" (TAGGED_MEM_STORE_REC_OPC | 8UL)          \
			: "memory");                                    \
})
#define	E2K_STORE_NULLPTR_QWORD(addr)                                   \
    _E2K_STORE_NULLPTR_QWORD(addr, TAGGED_MEM_STORE_REC_OPC)


#define	_E2K_STORE_NULLPTR_QWORD(addr, opc)                             \
({                                                                      \
    e2k_addr_t   addr_hi = (e2k_addr_t)addr + 8;                        \
    unsigned long np = 0UL;                                             \
    asm volatile ("{puttagd \t%0, %3, %0}\n"                            \
                  " {strd, 2 \t[%1 + %4], %0\n"                         \
                  "  strd, 5 \t[%2 + %4], %0}\n"                        \
                  : "+r" (np)                                           \
		  : "r" ((__e2k_ptr_t) (addr)),                         \
		    "r" ((__e2k_ptr_t) (addr_hi)),                      \
                    "i" (E2K_NULLPTR_ETAG),                             \
                    "i" ( (opc))                                        \
                   : "memory"                                           \
                 );                                                     \
})

#define E2K_MOVE_TAGGED_DWORD(from, to)	                \
    E2K_MOVE_TAGGED_DWORD_WITH_OPC(from, to, TAGGED_MEM_LOAD_REC_OPC, 0)

#define E2K_MOVE_TAGGED_QWORD_WITH_OPC(from, to, opc_lo, opc_hi)	\
({									\
	register long		__tmp_lo;				\
	register long		__tmp_hi;				\
	register e2k_addr_t	__to_hi = to + 8;			\
	asm ("{ldrd,0 [%2 + %5], %0\n"					\
	     " ldrd,2 [%2 + %6], %1}\n"					\
	     "{strd [%3 + %7], %0\n"					\
	     "strd [%4 + %7], %1}\n"					\
	     : "=&r" (__tmp_lo), "=&r" (__tmp_hi)			\
	     : "r" (from), "r" (to), "r" (__to_hi),			\
	       "ri" (opc_lo), "ri" (opc_hi),				\
	       "i" (TAGGED_MEM_STORE_REC_OPC)				\
	     : "memory");						\
})

#define E2K_MOVE_TAGGED_DWORD_WITH_OPC(from, to, opc,   \
                                       chan_letter)	\
({							\
	register long __tmp;				\
	asm (	"{ldrd," #chan_letter" [%1 + %3], %0}\n"\
		"strd [%2 + %4], %0\n"			\
		: "=&r" (__tmp)				\
		: "r" (from), "r" (to),			\
	         "ri" (opc),                            \
		 "i" (TAGGED_MEM_STORE_REC_OPC)	        \
		: "memory");	 			\
})

#define E2K_MOVE_TAGGED_DWORD_WITH_OPC_CH(from, to, opc, trap_cellar_chan) \
do { \
	switch (trap_cellar_chan) { \
	case 0: \
		E2K_MOVE_TAGGED_DWORD_WITH_OPC(from, to, opc, 0); \
		break; \
	case 1: \
		E2K_MOVE_TAGGED_DWORD_WITH_OPC(from, to, opc, 2); \
		break; \
	case 2: \
		E2K_MOVE_TAGGED_DWORD_WITH_OPC(from, to, opc, 3); \
		break; \
	case 3: \
		E2K_MOVE_TAGGED_DWORD_WITH_OPC(from, to, opc, 5); \
		break; \
	} \
} while (0)

/*
 * The same as E2K_MOVE_TAGGED_DWORD_WITH_OPC but does not touch
 * the lower 32-bits word.
 */
#define E2K_MOVE_TAGGED_WORD_WITH_OPC(from, to, opc,    \
                                       chan_letter)	\
({							\
	register long __tmp;				\
	register long __tmp1;				\
	asm volatile("ldrd [%3 + %5], %0\n"             \
	        "ldrd," #chan_letter" [%2 + %4], %1\n"  \
                "movts %0, %1\n"                        \
		"{strd [%3 + %6], %1}\n"		\
		: "+r" (__tmp),				\
		  "+r" (__tmp1)	        		\
		: "r" (from), "r" (to),			\
	         "r" (opc),                             \
		 "i" (TAGGED_MEM_LOAD_REC_OPC),	        \
		 "i" (TAGGED_MEM_STORE_REC_OPC)	        \
		: "memory");	 			\
})

#define E2K_MOVE_TAGGED_WORD_WITH_OPC_CH(from, to, opc, trap_cellar_chan) \
do { \
	switch (trap_cellar_chan) { \
	case 0: \
		E2K_MOVE_TAGGED_WORD_WITH_OPC(from, to, opc, 0); \
		break; \
	case 1: \
		E2K_MOVE_TAGGED_WORD_WITH_OPC(from, to, opc, 2); \
		break; \
	case 2: \
		E2K_MOVE_TAGGED_WORD_WITH_OPC(from, to, opc, 3); \
		break; \
	case 3: \
		E2K_MOVE_TAGGED_WORD_WITH_OPC(from, to, opc, 5); \
		break; \
	} \
} while (0)

#define E2K_TAGGED_MEMMOVE_8(__dst, __src)				\
({									\
	u_int64_t __tmp1;							\
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1)					\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC)		\
		: "memory");						\
})

#define E2K_TAGGED_MEMMOVE_16(__dst, __src)				\
({									\
	u_int64_t __tmp1, __tmp2;						\
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"strd,5 [ %[dst] + %[st_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1), [tmp2] "=&r" (__tmp2)		\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [ld_opc_8] "i" (TAGGED_MEM_LOAD_REC_OPC | 8),		\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [st_opc_8] "i" (TAGGED_MEM_STORE_REC_OPC | 8)		\
		: "memory");						\
})

#define E2K_TAGGED_MEMMOVE_24(__dst, __src)				\
({									\
	u_int64_t __tmp1, __tmp2, __tmp3;					\
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_16] ], %[tmp3]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"strd,5 [ %[dst] + %[st_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_16] ], %[tmp3]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1), [tmp2] "=&r" (__tmp2),		\
		  [tmp3] "=&r" (__tmp3)					\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [ld_opc_8] "i" (TAGGED_MEM_LOAD_REC_OPC | 8),		\
		  [ld_opc_16] "i" (TAGGED_MEM_LOAD_REC_OPC | 16),	\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [st_opc_8] "i" (TAGGED_MEM_STORE_REC_OPC | 8),	\
		  [st_opc_16] "i" (TAGGED_MEM_STORE_REC_OPC | 16)	\
		: "memory");						\
})

#define E2K_TAGGED_MEMMOVE_32(__dst, __src)				\
({									\
	u_int64_t __tmp1, __tmp2, __tmp3, __tmp4;				\
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_16] ], %[tmp3]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"strd,5 [ %[dst] + %[st_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_16] ], %[tmp3]\n"		\
		"strd,5 [ %[dst] + %[st_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1), [tmp2] "=&r" (__tmp2),		\
		  [tmp3] "=&r" (__tmp3), [tmp4] "=&r" (__tmp4)		\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [ld_opc_8] "i" (TAGGED_MEM_LOAD_REC_OPC | 8),		\
		  [ld_opc_16] "i" (TAGGED_MEM_LOAD_REC_OPC | 16),	\
		  [ld_opc_24] "i" (TAGGED_MEM_LOAD_REC_OPC | 24),	\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [st_opc_8] "i" (TAGGED_MEM_STORE_REC_OPC | 8),	\
		  [st_opc_16] "i" (TAGGED_MEM_STORE_REC_OPC | 16),	\
		  [st_opc_24] "i" (TAGGED_MEM_STORE_REC_OPC | 24)	\
		: "memory");						\
})

#define E2K_TAGGED_MEMMOVE_40(__dst, __src)				\
({									\
	u_int64_t __tmp1, __tmp2, __tmp3, __tmp4, __tmp5;			\
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_16] ], %[tmp3]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_32] ], %[tmp5]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"strd,5 [ %[dst] + %[st_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_16] ], %[tmp3]\n"		\
		"strd,5 [ %[dst] + %[st_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_32] ], %[tmp5]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1), [tmp2] "=&r" (__tmp2),		\
		  [tmp3] "=&r" (__tmp3), [tmp4] "=&r" (__tmp4),		\
		  [tmp5] "=&r" (__tmp5)					\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [ld_opc_8] "i" (TAGGED_MEM_LOAD_REC_OPC | 8),		\
		  [ld_opc_16] "i" (TAGGED_MEM_LOAD_REC_OPC | 16),	\
		  [ld_opc_24] "i" (TAGGED_MEM_LOAD_REC_OPC | 24),	\
		  [ld_opc_32] "i" (TAGGED_MEM_LOAD_REC_OPC | 32),	\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [st_opc_8] "i" (TAGGED_MEM_STORE_REC_OPC | 8),	\
		  [st_opc_16] "i" (TAGGED_MEM_STORE_REC_OPC | 16),	\
		  [st_opc_24] "i" (TAGGED_MEM_STORE_REC_OPC | 24),	\
		  [st_opc_32] "i" (TAGGED_MEM_STORE_REC_OPC | 32)	\
		: "memory");						\
})

#define E2K_TAGGED_MEMMOVE_48(__dst, __src)				\
({									\
	u_int64_t __tmp1, __tmp2, __tmp3, __tmp4, __tmp5, __tmp6;		\
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_16] ], %[tmp3]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_32] ], %[tmp5]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_40] ], %[tmp6]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"strd,5 [ %[dst] + %[st_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_16] ], %[tmp3]\n"		\
		"strd,5 [ %[dst] + %[st_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_32] ], %[tmp5]\n"		\
		"strd,5 [ %[dst] + %[st_opc_40] ], %[tmp6]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1), [tmp2] "=&r" (__tmp2),		\
		  [tmp3] "=&r" (__tmp3), [tmp4] "=&r" (__tmp4),		\
		  [tmp5] "=&r" (__tmp5), [tmp6] "=&r" (__tmp6)		\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [ld_opc_8] "i" (TAGGED_MEM_LOAD_REC_OPC | 8),		\
		  [ld_opc_16] "i" (TAGGED_MEM_LOAD_REC_OPC | 16),	\
		  [ld_opc_24] "i" (TAGGED_MEM_LOAD_REC_OPC | 24),	\
		  [ld_opc_32] "i" (TAGGED_MEM_LOAD_REC_OPC | 32),	\
		  [ld_opc_40] "i" (TAGGED_MEM_LOAD_REC_OPC | 40),	\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [st_opc_8] "i" (TAGGED_MEM_STORE_REC_OPC | 8),	\
		  [st_opc_16] "i" (TAGGED_MEM_STORE_REC_OPC | 16),	\
		  [st_opc_24] "i" (TAGGED_MEM_STORE_REC_OPC | 24),	\
		  [st_opc_32] "i" (TAGGED_MEM_STORE_REC_OPC | 32),	\
		  [st_opc_40] "i" (TAGGED_MEM_STORE_REC_OPC | 40)	\
		: "memory");						\
})

#define E2K_TAGGED_MEMMOVE_56(__dst, __src)				\
({									\
	u_int64_t __tmp1, __tmp2, __tmp3, __tmp4, __tmp5, __tmp6, __tmp7;	\
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_16] ], %[tmp3]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_32] ], %[tmp5]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_40] ], %[tmp6]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_48] ], %[tmp7]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"strd,5 [ %[dst] + %[st_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_16] ], %[tmp3]\n"		\
		"strd,5 [ %[dst] + %[st_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_32] ], %[tmp5]\n"		\
		"strd,5 [ %[dst] + %[st_opc_40] ], %[tmp6]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_48] ], %[tmp7]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1), [tmp2] "=&r" (__tmp2),		\
		  [tmp3] "=&r" (__tmp3), [tmp4] "=&r" (__tmp4),		\
		  [tmp5] "=&r" (__tmp5), [tmp6] "=&r" (__tmp6),		\
		  [tmp7] "=&r" (__tmp7)					\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [ld_opc_8] "i" (TAGGED_MEM_LOAD_REC_OPC | 8),		\
		  [ld_opc_16] "i" (TAGGED_MEM_LOAD_REC_OPC | 16),	\
		  [ld_opc_24] "i" (TAGGED_MEM_LOAD_REC_OPC | 24),	\
		  [ld_opc_32] "i" (TAGGED_MEM_LOAD_REC_OPC | 32),	\
		  [ld_opc_40] "i" (TAGGED_MEM_LOAD_REC_OPC | 40),	\
		  [ld_opc_48] "i" (TAGGED_MEM_LOAD_REC_OPC | 48),	\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [st_opc_8] "i" (TAGGED_MEM_STORE_REC_OPC | 8),	\
		  [st_opc_16] "i" (TAGGED_MEM_STORE_REC_OPC | 16),	\
		  [st_opc_24] "i" (TAGGED_MEM_STORE_REC_OPC | 24),	\
		  [st_opc_32] "i" (TAGGED_MEM_STORE_REC_OPC | 32),	\
		  [st_opc_40] "i" (TAGGED_MEM_STORE_REC_OPC | 40),	\
		  [st_opc_48] "i" (TAGGED_MEM_STORE_REC_OPC | 48)	\
		: "memory");						\
})

#define E2K_TAGGED_MEMMOVE_64(__dst, __src)				\
({									\
	u_int64_t __tmp1, __tmp2, __tmp3, __tmp4, __tmp5, __tmp6, __tmp7, __tmp8; \
	asm volatile (							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_0] ], %[tmp1]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_16] ], %[tmp3]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_32] ], %[tmp5]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_40] ], %[tmp6]\n"		\
		"}\n"							\
		"{\n"							\
		"ldrd,2 [ %[src] + %[ld_opc_48] ], %[tmp7]\n"		\
		"ldrd,5 [ %[src] + %[ld_opc_56] ], %[tmp8]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_0] ], %[tmp1]\n"		\
		"strd,5 [ %[dst] + %[st_opc_8] ], %[tmp2]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_16] ], %[tmp3]\n"		\
		"strd,5 [ %[dst] + %[st_opc_24] ], %[tmp4]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_32] ], %[tmp5]\n"		\
		"strd,5 [ %[dst] + %[st_opc_40] ], %[tmp6]\n"		\
		"}\n"							\
		"{\n"							\
		"strd,2 [ %[dst] + %[st_opc_48] ], %[tmp7]\n"		\
		"strd,5 [ %[dst] + %[st_opc_56] ], %[tmp8]\n"		\
		"}\n"							\
		: [tmp1] "=&r" (__tmp1), [tmp2] "=&r" (__tmp2),		\
		  [tmp3] "=&r" (__tmp3), [tmp4] "=&r" (__tmp4),		\
		  [tmp5] "=&r" (__tmp5), [tmp6] "=&r" (__tmp6),		\
		  [tmp7] "=&r" (__tmp7), [tmp8] "=&r" (__tmp8)		\
		: [src] "r" (__src), [dst] "r" (__dst),			\
		  [ld_opc_0] "i" (TAGGED_MEM_LOAD_REC_OPC),		\
		  [ld_opc_8] "i" (TAGGED_MEM_LOAD_REC_OPC | 8),		\
		  [ld_opc_16] "i" (TAGGED_MEM_LOAD_REC_OPC | 16),	\
		  [ld_opc_24] "i" (TAGGED_MEM_LOAD_REC_OPC | 24),	\
		  [ld_opc_32] "i" (TAGGED_MEM_LOAD_REC_OPC | 32),	\
		  [ld_opc_40] "i" (TAGGED_MEM_LOAD_REC_OPC | 40),	\
		  [ld_opc_48] "i" (TAGGED_MEM_LOAD_REC_OPC | 48),	\
		  [ld_opc_56] "i" (TAGGED_MEM_LOAD_REC_OPC | 56),	\
		  [st_opc_0] "i" (TAGGED_MEM_STORE_REC_OPC),		\
		  [st_opc_8] "i" (TAGGED_MEM_STORE_REC_OPC | 8),	\
		  [st_opc_16] "i" (TAGGED_MEM_STORE_REC_OPC | 16),	\
		  [st_opc_24] "i" (TAGGED_MEM_STORE_REC_OPC | 24),	\
		  [st_opc_32] "i" (TAGGED_MEM_STORE_REC_OPC | 32),	\
		  [st_opc_40] "i" (TAGGED_MEM_STORE_REC_OPC | 40),	\
		  [st_opc_48] "i" (TAGGED_MEM_STORE_REC_OPC | 48),	\
		  [st_opc_56] "i" (TAGGED_MEM_STORE_REC_OPC | 56)	\
		: "memory");						\
})


/* Store quadro pointer "ptr" at address "addr" */
#define E2K_SET_TAGS_AND_STORE_QUADRO(ptr, addr) \
do { \
	asm ("{\n" \
	     "puttagd %0, 15, %%db[0]\n" \
	     "puttagd %1, 12, %%db[1]\n" \
	     "}\n" \
	     "{\n" \
	     "strd,2 [ %2 + %3 ], %%db[0]\n" \
	     "strd,5 [ %2 + %4 ], %%db[1]\n" \
	     "}\n" \
	     : \
	     : "r" (AW(ptr).lo), "r" (AW(ptr).hi), \
	       "r" ((unsigned long) addr), \
	       "i" (TAGGED_MEM_STORE_REC_OPC), \
	       "i" (TAGGED_MEM_STORE_REC_OPC | 8UL) \
	     : "%b[0]", "%b[1]"); \
} while (0)

#define E2K_LOAD_VAL_AND_TAGD_WITH_OPC(addr, opc, val, chan_letter, tag)\
({									\
	register int __dtag;						\
	register long __dword;						\
	asm volatile ("{ldrd," #chan_letter "\t[%2 + %3], %0\n}"	\
			" gettagd \t%0, %1\n"				\
			" puttags \t%0, 0, %0\n"			\
			: "+r"(__dword),				\
			  "=r"(__dtag)					\
			: "r" ((__e2k_ptr_t) (addr)),			\
			  "r" ((uint64_t) (opc)));			\
	val = __dword;							\
	tag = __dtag;							\
})


/*
 * Read tags at @src and pack them at @dst.
 */
#define E2K_EXTRACT_TAGS_32(dst, src) \
do { \
	register u_int64_t __opc0 = TAGGED_MEM_LOAD_REC_OPC; \
	register u_int64_t __opc8 = TAGGED_MEM_LOAD_REC_OPC | 8; \
	register u_int64_t __opc16 = TAGGED_MEM_LOAD_REC_OPC | 16; \
	register u_int64_t __opc24 = TAGGED_MEM_LOAD_REC_OPC | 24; \
	register u_int64_t __tmp0, __tmp8, __tmp16, __tmp24; \
 \
	asm volatile (	"{\n" \
			"ldrd,0 [%5 + %6], %0\n" \
			"ldrd,2 [%5 + %7], %1\n" \
			"ldrd,3 [%5 + %8], %2\n" \
			"ldrd,5 [%5 + %9], %3\n" \
			"}\n" \
			"{\n" \
			"gettagd,2 %1, %1\n" \
			"gettagd,5 %3, %3\n" \
			"}\n" \
			"{\n" \
			"gettagd,2 %0, %0\n" \
			"gettagd,5 %2, %2\n" \
			"shls,0 %1, 4, %1\n" \
			"shls,3 %3, 4, %3\n" \
			"}\n" \
			"{\n" \
			"ors,0 %0, %1, %0\n" \
			"ors,3 %2, %3, %2\n" \
			"}\n" \
			"{\n" \
			"stb,2 [ %4 + 0 ], %0\n" \
			"stb,5 [ %4 + 1 ], %2\n" \
			"}\n" \
			: "=&r" (__tmp0), "=&r" (__tmp8), \
			  "=&r" (__tmp16), "=&r" (__tmp24) \
			: "r" (dst), "r" (src), \
			  "r" (__opc0), "r" (__opc8), \
			  "r" (__opc16), "r" (__opc24)); \
} while (0)

#define E2K_LOAD_TAGD(addr)                             \
({                                                      \
        register int __dtag;                            \
        register long __dword;                          \
 	asm volatile ("{ldrd [%2 + %3], %1\n}"          \
                       "gettagd \t%1, %0\n"             \
                      : "=r"(__dtag),                   \
                        "=r"(__dword)                   \
 			: "r" (addr),                   \
 			  "i"(TAGGED_MEM_LOAD_REC_OPC));\
     __dtag;                                            \
})

#define E2K_LOAD_TAGS(addr)                             \
({                                                      \
        register int __dtag;                            \
        register long __dword;                          \
 	asm volatile ("{ldrd [%2 + %3], %1\n}"          \
                       "gettags \t%1, %0\n"             \
                      : "=r"(__dtag),                   \
                        "=r"(__dword)                   \
 			: "r" (addr),                   \
 			  "i"(TAGGED_MEM_LOAD_REC_OPC));\
     __dtag;                                            \
})

#define E2K_LOAD_VAL_AND_TAGD_OPC(addr, opc, val, tag)  \
({                                                      \
	register int __dtag;                            \
	register long __dword;                          \
	asm volatile ("{ldrd [%2 + %3], %1\n}"          \
		      "{gettagd \t%1, %0\n"		\
		      " puttagd \t%1, 0, %1}\n"		\
		      : "=r"(__dtag),                   \
			"=r"(__dword)                   \
		      : "r" (addr),			\
			"ri"(opc));			\
	val = __dword;					\
	tag = __dtag;					\
})


#define E2K_LOAD_VAL_AND_TAGD(addr, val, tag)           \
({                                                      \
	register int __dtag;                            \
	register long __dword;                          \
	asm ("{ldrd [%2 + %3], %1\n}"			\
	     "{gettagd \t%1, %0\n"			\
	     " puttagd \t%1, 0, %1}\n"			\
	     : "=r"(__dtag), "=r"(__dword)		\
	     : "m" (*((unsigned long long *) (addr))),	\
	       "i"(TAGGED_MEM_LOAD_REC_OPC));		\
	val = __dword;					\
	tag = __dtag;					\
})

#define E2K_LOAD_VAL_AND_TAGD_CH(addr, val, tag, ch)	\
({                                                      \
	register int __dtag;                            \
	register long __dword;                          \
	asm ("{ldrd," #ch " [%2 + %3], %1\n}"		\
	     "{gettagd \t%1, %0\n"			\
	     " puttagd \t%1, 0, %1}\n"			\
	     : "=r"(__dtag), "=r"(__dword)		\
	     : "m" (*((unsigned long long *) (addr))),	\
	       "i"(TAGGED_MEM_LOAD_REC_OPC));		\
	val = __dword;					\
	tag = __dtag;					\
})

#define	E2K_LOAD_TAGGED_QWORD_AND_TAGS(addr, lo,                \
                                       hi, tag_lo, tag_hi)	\
		{E2K_LOAD_VAL_AND_TAGD(addr, lo, tag_lo);       \
		E2K_LOAD_VAL_AND_TAGD(((uint64_t *) (addr)) + 1,\
                                      hi, tag_hi);}

/**
 * Получить тег операции двойного слова (регистр). Get tag of double-word (register) operation.
 */
#define E2K_GETTAGS(src) \
({ \
    register int res; \
    asm volatile ("gettags \t%1, %0" \
                  : "=r" (res) \
                  : "r" ((uint32_t) (src)) \
		 ); \
    res; \
})

#define E2K_GETTAGD(src) \
({ \
    register int res; \
    asm volatile ("gettagd \t%1, %0" \
                  : "=r" (res) \
                  : "r" ((uint64_t) (src)) \
		 ); \
    res; \
})


#define E2K_PUTTAGS(dst, tag) \
({ \
    E2K_BUILD_BUG_ON(sizeof(dst) != 4); \
    asm volatile ("puttags \t%1, %2, %0" \
                  : "=r" (dst) \
                  : "r" (dst), \
                    "ri" ((uint32_t) (tag)) \
		 ); \
})

#define E2K_PUTTAGD(dst, tag) \
({ \
    E2K_BUILD_BUG_ON(sizeof(dst) != 8); \
    asm volatile ("puttagd \t%1, %2, %0" \
                  : "=r" (dst) \
                  : "r" (dst), \
                    "ri" ((uint32_t) (tag)) \
		 ); \
})

/*
 * Put a tag on a given general purpose register.
 */
#define E2K_PUTTAGD_R(reg_no, tag) \
({ \
	asm volatile ("puttagd \t %%dr" #reg_no ", %0, %%dr" #reg_no \
		: \
		: "ri" (tag)); \
})

/*
 * Bytes swapping
 */

#define	E2K_SWAPB_16(addr)	E2K_READ_MAS_H(addr, MAS_BIGENDIAN)
#define	E2K_SWAPB_32(addr)	E2K_READ_MAS_W(addr, MAS_BIGENDIAN)
#define	E2K_SWAPB_64(addr)	E2K_READ_MAS_D(addr, MAS_BIGENDIAN)

#define _E2K_GEN_LABEL(label_name, label_no)  #label_name #label_no

#define _E2K_ASM_LABEL_L(label_name, label_no) \
    asm volatile ("\n" _E2K_GEN_LABEL(label_name, label_no) ":");

#define _E2K_ASM_LABEL_R(label_name, label_no) \
    _E2K_GEN_LABEL(label_name, label_no)


/*
 * Left and right brackets for atomic operations
 */
#define _E2K_ATOMIC_LBRACKET(addr, mas, label_no, type, size_letter) \
({ \
    _E2K_ASM_LABEL_L (, label_no); \
    _E2K_READ_MAS ((addr), (mas), type, size_letter, 0); \
})

#define _E2K_ATOMIC_RBRACKET(addr, val, mas, label_no, type, size_letter) \
({ \
    asm volatile ("{"); \
    _E2K_WRITE_MAS ((addr), (val), (mas), type, size_letter, 2); \
    asm volatile ("ibranch " _E2K_ASM_LABEL_R (, label_no) " ? %MLOCK"); \
    asm volatile ("}\n\n"); \
})

#define E2K_ATOMIC_LBRACKET_B(addr, mas, label_no)  _E2K_ATOMIC_LBRACKET ((addr), (mas), label_no, uint8_t, b)
#define E2K_ATOMIC_LBRACKET_H(addr, mas, label_no)  _E2K_ATOMIC_LBRACKET ((addr), (mas), label_no, uint16_t, h)
#define E2K_ATOMIC_LBRACKET_W(addr, mas, label_no)  _E2K_ATOMIC_LBRACKET ((addr), (mas), label_no, uint32_t, w)
#define E2K_ATOMIC_LBRACKET_D(addr, mas, label_no)  _E2K_ATOMIC_LBRACKET ((addr), (mas), label_no, uint64_t, d)

#define E2K_ATOMIC_RBRACKET_B(addr, val, mas, label_no)  _E2K_ATOMIC_RBRACKET (addr, val, mas, label_no, uint8_t, b)
#define E2K_ATOMIC_RBRACKET_H(addr, val, mas, label_no)  _E2K_ATOMIC_RBRACKET (addr, val, mas, label_no, uint16_t, h)
#define E2K_ATOMIC_RBRACKET_W(addr, val, mas, label_no)  _E2K_ATOMIC_RBRACKET (addr, val, mas, label_no, uint32_t, w)
#define E2K_ATOMIC_RBRACKET_D(addr, val, mas, label_no)  _E2K_ATOMIC_RBRACKET (addr, val, mas, label_no, uint64_t, d)


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

#ifdef CONFIG_DEBUG_LCC_VOLATILE_ATOMIC
# define NOT_VOLATILE volatile
#else
# define NOT_VOLATILE
#endif

#ifdef CONFIG_ATOMICS_5CH_ACQUIRE
# define ACQUIRE_ATOMIC_CHANNEL "5"
#else
# define ACQUIRE_ATOMIC_CHANNEL "2"
#endif

#ifdef CONFIG_ISET_V5_SUPPORTED
# define RELAXED_ATOMIC_CHANNEL "5"
#else
# define RELAXED_ATOMIC_CHANNEL "2"
#endif


#define __api_atomic32_add_if_not_negative(__val, __addr) \
({ \
	int __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{" \
			  "\n\tnop 1" \
			  "\n\tcmplsb %[rval], 0, %%pred2" \
			  "\n\t}" \
			  "\n\tadds %[rval], %[val], %[rval] ? ~ %%pred2" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory", "pred2");	\
	__rval; \
})

#define __api_atomic64_add_if_not_negative(__val, __addr) \
({ \
	long long __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{" \
			  "\n\tnop 1" \
			  "\n\tcmpldb %[rval], 0, %%pred2" \
			  "\n\t}" \
			  "\n\taddd %[rval], %[val], %[rval] ? ~ %%pred2" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory", "pred2");	\
	__rval; \
})

/*
 * if ((unsigned) *addr < (unsigned) limit)
 *	*addr += val;
 * return *addr;
 */
#define __api_atomic64_add_if_below(__val, __addr, __limit) \
({ \
	long long __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{" \
			  "\n\tnop 1" \
			  "\n\tcmpbdb %[rval], %[limit], %%pred2" \
			  "\n\t}" \
			  "\n\taddd %[rval], %[val], %[rval] ? %%pred2" \
			  "\n\t{" \
			  "\n\tstd,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val), [limit] "ir" (__limit) \
			  : "memory", "pred2");	\
	__rval; \
})


/* Atomically add and return the old value */
#define __api_atomic32_add_oldval(__val, __addr) \
({ \
	int __rval, __tmp; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tadds %[rval], %[val], %[tmp]" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[tmp], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [tmp] "=&r" (__tmp), \
			    [addr] "+m" (*(__addr)) \
			  : [val] "i" (__val) \
			  : "memory");	\
	__rval; \
})

#define __api_atomic32_add_oldval_acq(__val, __addr) \
({ \
	int __rval, __tmp; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tadds %[rval], %[val], %[tmp]" \
			  "\n\t{"\
			  "\n\tstw," ACQUIRE_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [tmp] "=&r" (__tmp), \
			    [addr] "+m" (*(__addr)) \
			  : [val] "i" (__val) \
			  : "memory");	\
	__rval; \
})


/* Atomically sub and return the old value */
#define __api_atomic64_sub_oldval(__val, __addr) \
({ \
 	long long __rval, __tmp; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\tsubd %[rval], %[val], %[tmp]" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[tmp], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [tmp] "=&r" (__tmp), \
			    [addr] "+m" (*(__addr)) \
			  : [val] "i" (__val) \
			  : "memory");	\
	__rval; \
})


/*
 * C equivalent:
 *
 *	spinlock_t oldval, newval;
 *	oldval.lock = ACCESS_ONCE(lock->lock);
 *	if (oldval.head == oldval.tail) {
 *		newval.lock = oldval.lock + (1 << ARCH_SPINLOCK_TAIL_SHIFT);
 *		if (cmpxchg(&lock->lock, oldval.lock, newval.lock) ==
 *				oldval.lock)
 *			return 1;
 *	}
 *	return 0;
 */
#define __api_atomic_ticket_trylock(spinlock, tail_shift) \
({ \
 	register int	__rval;	\
	register int	__val; \
	register int	__head; \
	register int	__tail; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[val], mas=0x7" \
			  "\n\t{" \
			  "\n\tshrs,0 %[val], 0x10, %[tail]" \
			  "\n\tgetfs,1 %[val], 0x400, %[head]" \
			  "\n\t}" \
			  "\n\t{" \
			  "\n\tnop" \
			  "\n\tcmpesb,0 %[tail], %[head], %%pred2" \
			  "\n\tadds 0, 0, %[rval]" \
			  "\n\t}" \
			  "\n\t{" \
			  "\n\tadds,0 0, 1, %[rval] ? %%pred2" \
			  "\n\tadds,2 %[val], %[incr], %[val] ? %%pred2" \
			  "\n\t}" \
			  "\n\t{" \
			  "\n\tstw," ACQUIRE_ATOMIC_CHANNEL " %[addr], %[val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [val] "=&r" (__val), \
			    [head] "=&r" (__head), [tail] "=&r" (__tail), \
			    [addr] "+m" (*(spinlock)) \
			  : [incr] "i" (1 << tail_shift) \
			  : "memory", "pred2"); \
	__rval; \
})


/*
 * Atomic operations without return value and acquire/release semantics
 */

#define __api_atomic32_add(__val, __addr) \
({ \
	int __tmp; \
	asm ("\n1:" \
	     "\n\tldw,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tadds %[tmp], %[val], %[tmp]" \
	     "\n\t{"\
	     "\n\tstw," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [val] "ir" (__val)); \
})

#define __api_atomic64_add(__val, __addr) \
({ \
	long long __tmp; \
	asm ("\n1:" \
	     "\n\tldd,0 %[addr], %[tmp], mas=0x7" \
	     "\n\taddd %[tmp], %[val], %[tmp]" \
	     "\n\t{"\
	     "\n\tstd," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [val] "ir" (__val)); \
})

#define __api_atomic32_sub(__val, __addr) \
({ \
	int __tmp; \
	asm ("\n1:" \
	     "\n\tldw,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tsubs %[tmp], %[val], %[tmp]" \
	     "\n\t{"\
	     "\n\tstw," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [val] "ir" (__val)); \
})

#define __api_atomic64_sub(__val, __addr) \
({ \
	long long __tmp; \
	asm ("\n1:" \
	     "\n\tldd,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tsubd %[tmp], %[val], %[tmp]" \
	     "\n\t{"\
	     "\n\tstd," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [val] "ir" (__val)); \
})

#define __api_atomic8_set_mask(__mask, __addr) \
({ \
	char __tmp; \
	asm ("\n1:" \
	     "\n\tldb,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tors %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstb," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic16_set_mask(__mask, __addr) \
({ \
	short __tmp; \
	asm ("\n1:" \
	     "\n\tldh,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tors %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tsth," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic32_set_mask(__mask, __addr) \
({ \
	int __tmp; \
	asm ("\n1:" \
	     "\n\tldw,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tors %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstw," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic64_set_mask(__mask, __addr) \
({ \
	long long __tmp; \
	asm ("\n1:" \
	     "\n\tldd,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tord %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstd," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})

#define __api_atomic8_clear_mask(__mask, __addr) \
({ \
	char __tmp; \
	asm ("\n1:" \
	     "\n\tldb,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tandns %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstb," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic16_clear_mask(__mask, __addr) \
({ \
	short __tmp; \
	asm ("\n1:" \
	     "\n\tldh,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tandns %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tsth," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic32_clear_mask(__mask, __addr) \
({ \
	int __tmp; \
	asm ("\n1:" \
	     "\n\tldw,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tandns %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstw," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic64_clear_mask(__mask, __addr) \
({ \
	long long __tmp; \
	asm ("\n1:" \
	     "\n\tldd,0 %[addr], %[tmp], mas=0x7" \
	     "\n\tandnd %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstd," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})

#define __api_atomic8_change_mask(__mask, __addr) \
({ \
	char __tmp; \
	asm ("\n1:" \
	     "\n\tldb,0 %[addr], %[tmp], mas=0x7" \
	     "\n\txors %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstb," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic16_change_mask(__mask, __addr) \
({ \
	short __tmp; \
	asm ("\n1:" \
	     "\n\tldh,0 %[addr], %[tmp], mas=0x7" \
	     "\n\txors %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tsth," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic32_change_mask(__mask, __addr) \
({ \
	int __tmp; \
	asm ("\n1:" \
	     "\n\tldw,0 %[addr], %[tmp], mas=0x7" \
	     "\n\txors %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstw," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})
#define __api_atomic64_change_mask(__mask, __addr) \
({ \
	long long __tmp; \
	asm ("\n1:" \
	     "\n\tldd,0 %[addr], %[tmp], mas=0x7" \
	     "\n\txord %[tmp], %[mask], %[tmp]" \
	     "\n\t{"\
	     "\n\tstd," RELAXED_ATOMIC_CHANNEL " %[addr], %[tmp], mas=0x2" \
	     "\n\tibranch 1b ? %%MLOCK" \
	     "\n\t}" \
	     : [tmp] "=&r" (__tmp), [addr] "+m" (*(__addr)) \
	     : [mask] "ir" (__mask)); \
})


/*
 * Atomic operations with return value and acquire/release semantics
 */

#define __api_atomic32_add_unless_return(__val, __addr, __unless) \
({ \
	int __rval, __tmp; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{"\
			  "\n\tnop" \
			  "\n\tcmpesb %[rval], %[unless], %%pred2" \
			  "\n\t}" \
			  "\n\t{"\
			  "\n\tadds %[rval], %[val], %[tmp] ? ~ %%pred2" \
			  "\n\tadds %[rval], 0, %[tmp] ? %%pred2" \
			  "\n\t}" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[tmp], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [tmp] "=&r" (__tmp), \
			    [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val), [unless] "ir" (__unless) \
			  : "memory", "pred2"); \
	__rval; \
})

#define __api_atomic64_add_unless_return(__val, __addr, __unless) \
({ \
	long long __rval, __tmp; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{"\
			  "\n\tnop" \
			  "\n\tcmpedb %[rval], %[unless], %%pred2" \
			  "\n\t}" \
			  "\n\t{"\
			  "\n\taddd %[rval], %[val], %[tmp] ? ~ %%pred2" \
			  "\n\taddd %[rval], 0, %[tmp] ? %%pred2" \
			  "\n\t}" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[tmp], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [tmp] "=&r" (__tmp), \
			    [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val), [unless] "ir" (__unless) \
			  : "memory", "pred2"); \
	__rval; \
})

#define __api_atomic8_add_return(__val, __addr) \
({ \
	char __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldb,0 %[addr], %[rval], mas=0x7" \
			  "\n\tadds %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tstb,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic16_add_return(__val, __addr) \
({ \
	short __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldh,0 %[addr], %[rval], mas=0x7" \
			  "\n\tadds %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tsth,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic32_add_return(__val, __addr) \
({ \
	int __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tadds %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic64_add_return(__val, __addr) \
({ \
	long long __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\taddd %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic32_sub_return(__val, __addr) \
({ \
	int __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tsubs %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic32_sub_return_acq(__val, __addr) \
({ \
	int __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tsubs %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tstw," ACQUIRE_ATOMIC_CHANNEL " %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic64_sub_return(__val, __addr) \
({ \
	long long __rval; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\tsubd %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic8_get_old_set_mask(__mask, __addr) \
({ \
	char __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldb,0 %[addr], %[rval], mas=0x7" \
			  "\n\tors %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstb,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic16_get_old_set_mask(__mask, __addr) \
({ \
	short __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldh,0 %[addr], %[rval], mas=0x7" \
			  "\n\tors %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tsth,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic32_get_old_set_mask(__mask, __addr) \
({ \
	int __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tors %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic64_get_old_set_mask(__mask, __addr) \
({ \
	long long __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\tord %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic8_get_old_clear_mask(__mask, __addr) \
({ \
	char __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldb,0 %[addr], %[rval], mas=0x7" \
			  "\n\tandns %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstb,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic16_get_old_clear_mask(__mask, __addr) \
({ \
	short __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldh,0 %[addr], %[rval], mas=0x7" \
			  "\n\tandns %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tsth,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic32_get_old_clear_mask(__mask, __addr) \
({ \
	int __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tandns %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic64_get_old_clear_mask(__mask, __addr) \
({ \
	long long __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\tandnd %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})

#define __api_atomic8_get_old_change_mask(__mask, __addr) \
({ \
	char __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldb,0 %[addr], %[rval], mas=0x7" \
			  "\n\txors %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstb,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic16_get_old_change_mask(__mask, __addr) \
({ \
	short __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldh,0 %[addr], %[rval], mas=0x7" \
			  "\n\txors %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tsth,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic32_get_old_change_mask(__mask, __addr) \
({ \
	int __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\txors %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})
#define __api_atomic64_get_old_change_mask(__mask, __addr) \
({ \
	long long __rval, __stored_val; \
	asm NOT_VOLATILE ("\n1:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\txord %[rval], %[mask], %[stored_val]" \
			  "\n\t{"\
			  "\n\tstd,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [mask] "ir" (__mask) \
			  : "memory"); \
	__rval; \
})

#define __api_xchg_return(__val, __addr, size_letter) \
({ \
	long long __rval;	\
	asm NOT_VOLATILE ("\n2:" \
			  "\n\tld"#size_letter ",0 %[addr], %[rval], mas=0x7" \
			  "\n\t{"\
			  "\n\tst"#size_letter ",2 %[addr], %[val], mas=0x2" \
			  "\n\tibranch 2b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "r" (__val) \
			  : "memory"); \
	__rval; \
})

#define __api_cmpxchg_return(__old, __new, __addr, size_letter, sxt_size) \
({ \
	long long __rval, __stored_val; \
	asm NOT_VOLATILE ("\n3:" \
			  "\n\tld"#size_letter ",0 %[addr], %[rval], mas=0x7" \
			  "\n\t{" \
			  "\n\tsxt\t"#sxt_size", %[rval], %[rval]" \
			  "\n\taddd 0x0, %[new], %[stored_val]" \
			  "\n\t}" \
			  "\n\t{" \
			  "\n\tnop 1" \
			  "\n\tcmpedb %[rval], %[old], %%pred2" \
			  "\n\t}" \
			  "\n\taddd 0x0, %[rval], %[stored_val] ? ~ %%pred2" \
			  "\n\t{" \
			  "\n\tst"#size_letter ",2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 3b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [stored_val] "=&r" (__stored_val), \
			    [addr] "+m" (*(__addr)) \
			  : [new] "ir" (__new), [old] "ir" (__old) \
			  : "memory", "pred2");	\
	__rval; \
})

#define __api_cmpxchg_word_return(__old, __new, __addr) \
({ \
	long __rval, __stored_val; \
	asm NOT_VOLATILE ("\n3:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{" \
			  "\n\tnop 1" \
			  "\n\tadds 0x0, %[new], %[stored_val]" \
			  "\n\tcmpesb %[rval], %[old], %%pred2" \
			  "\n\t}" \
			  "\n\tadds 0x0, %[rval], %[stored_val] ? ~ %%pred2" \
			  "\n\t{" \
			  "\n\tstw,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 3b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [new] "ir" (__new), [old] "ir" (__old) \
			  : "memory", "pred2"); \
	__rval; \
})

#define __api_cmpxchg_dword_return(__old, __new, __addr) \
({ \
	long long __rval, __stored_val; \
	asm NOT_VOLATILE ("\n3:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{" \
			  "\n\tnop 1" \
			  "\n\taddd 0x0, %[new], %[stored_val]" \
			  "\n\tcmpedb %[rval], %[old], %%pred2" \
			  "\n\t}" \
			  "\n\taddd 0x0, %[rval], %[stored_val] ? ~ %%pred2" \
			  "\n\t{" \
			  "\n\tstd,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 3b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [new] "ir" (__new), [old] "ir" (__old) \
			  : "memory", "pred2"); \
	__rval; \
})

#define __api_cmpxchg_dword_return_relaxed(__old, __new, __addr) \
({ \
	long long __rval, __stored_val; \
	asm NOT_VOLATILE ("\n3:" \
			  "\n\tldd,0 %[addr], %[rval], mas=0x7" \
			  "\n\t{" \
			  "\n\tnop 1" \
			  "\n\taddd 0x0, %[new], %[stored_val]" \
			  "\n\tcmpedb %[rval], %[old], %%pred2" \
			  "\n\t}" \
			  "\n\taddd 0x0, %[rval], %[stored_val] ? ~ %%pred2" \
			  "\n\t{" \
			  "\n\tstd,2 %[addr], %[stored_val], mas=0x2" \
			  "\n\tibranch 3b ? %%MLOCK" \
			  "\n\t}" \
			  : [stored_val] "=&r" (__stored_val), \
			    [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [new] "ir" (__new), [old] "ir" (__old) \
			  : "memory", "pred2"); \
	__rval; \
})

#define __api_xchg8_return(val, addr) \
			__api_xchg_return(val, addr, b)
	
#define __api_xchg16_return(val, addr) \
			__api_xchg_return(val, addr, h)
	
#define __api_xchg32_return(val, addr) \
			__api_xchg_return(val, addr, w)
	
#define __api_xchg64_return(val, addr) \
			__api_xchg_return(val, addr, d)
	
#define __api_cmpxchg8_return(old, new, addr) \
			__api_cmpxchg_return(old, new, addr, b, 0x4)

#define __api_cmpxchg16_return(old, new, addr) \
			__api_cmpxchg_return(old, new, addr, h, 0x5)
	
#define __api_cmpxchg32_return(old, new, addr) \
			__api_cmpxchg_word_return(old, new, addr)
	
#define __api_cmpxchg64_return(old, new, addr) \
			__api_cmpxchg_dword_return(old, new, addr)


/*
 * Bit operations (for E3S and more modern processors)
 */

/* Put bits in reverse order */
#define E2K_BITREVS(val) \
({ \
	register uint32_t __res; \
	asm ("bitrevs %1, %0" : "=r" (__res)  : "r" (val)); \
	__res; \
})

#define E2K_BITREVD(val) \
({ \
	register uint64_t __res; \
	asm ("bitrevd %1, %0" : "=r" (__res)  : "r" (val)); \
	__res; \
})

/* Get number of leading zeroes */
#define E2K_LZCNTS(val) \
({ \
	register uint32_t __res; \
	asm ("lzcnts %1, %0" : "=r" (__res)  : "r" (val)); \
	__res; \
})

#define E2K_LZCNTD(val) \
({ \
	register uint64_t __res; \
	asm ("lzcntd %1, %0" : "=r" (__res)  : "r" (val)); \
	__res; \
})

/* Get number of 1's */
#define E2K_POPCNTS(val) \
({ \
	register uint32_t __res; \
	asm ("popcnts %1, %0" : "=r" (__res)  : "r" (val)); \
	__res; \
})

#define E2K_POPCNTD(val) \
({ \
	register uint64_t __res; \
	asm ("popcntd %1, %0" : "=r" (__res)  : "r" (val)); \
	__res; \
})

#define E2K_WAIT_V5(num) \
({ \
	asm volatile ("{wait sal=%8, sas=%7, trap=%6, ma_c=%5, fl_c=%4, "\
			"ld_c = %3, st_c=%2, all_e=%1, all_c=%0}" \
		      : \
		      : "i" (((num) & 0x1)), \
			"i" (((num) & 0x2)  >> 1), \
			"i" (((num) & 0x4)  >> 2), \
			"i" (((num) & 0x8)  >> 3), \
			"i" (((num) & 0x10) >> 4), \
			"i" (((num) & 0x20) >> 5), \
			"i" (((num) & 0x40) >> 6), \
			"i" (((num) & 0x80) >> 7), \
			"i" (((num) & 0x100) >> 8) \
		      : "memory" ); \
})

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

/* 
 * Wait for es2 - difference with arch e2k is wait trap support (C.22.1) 
 */
#define E2K_WAIT_ES2(num) \
({ \
    _Pragma ("no_asm_inline")						\
    asm volatile ("wait \ttrap = %0, ma_c = %1, fl_c = %2, ld_c = %3, "	\
				"st_c = %4, all_e = %5, all_c = %6"	\
		  :	\
		  : "i" (((num) & 0x40) >> 6),	\
		    "i" (((num) & 0x20) >> 5),	\
		    "i" (((num) & 0x10) >> 4),	\
		    "i" (((num) & 0x8)  >> 3),	\
		    "i" (((num) & 0x4)  >> 2),	\
		    "i" (((num) & 0x2)  >> 1),	\
		    "i" (((num) & 0x1))	\
		 :  "memory" );	\
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

#define _sal	0x100	/* store-after-load modifier for _lt_c */
#define _sas	0x80	/* store-after-store modifier for _st_c */
#define _trap	0x40	/* stop the conveyor untill interrupt */
#define _ma_c	0x20
#define _fl_c	0x10
#define _ld_c	0x8	/* stop until all load operations complete */
#define _st_c	0x4	/* stop until store operations complete */
#define _all_e	0x2
#define _all_c	0x1

#define wtrap()	E2K_WAIT_ES2(_trap)
/* This must clobber everything that could change on interrupt,
 * including some global registers if they differ on different CPUs. */
#define IRQ_BARRIER_CLOBBERS "memory"

#define E2K_FLUSHTS			\
({					\
	_Pragma("no_asm_inline")	\
	asm volatile ("flushts");	\
})

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

#define E2K_FLUSH_ALL_TC \
({ \
	register uint64_t res; \
	asm volatile ("{nop 3; invtc 0x0, %0}" \
		      : "=r" (res)); \
	res; \
})

#define E2K_GET_TCD() \
({ \
    register uint64_t res; \
    asm volatile ( \
    		"\n\t{gettc \t0x1 , %%ctpr1; nop 5}" \
		"\n\trrd \t%%ctpr1, %0" \
		: "=r" (res) : : "ctpr1" ); \
    res; \
})

#define E2K_SET_TCD(val) \
({ \
    asm volatile ("{puttc %0, 0x0 , %%tcd}" \
	    	   : \
		   :"r" (val)); \
})

#define E2K_BUBBLE(num) \
do { \
	asm volatile ("{nop %0}" \
			: \
			: "i" (num & 0x7) \
			: "memory"); \
} while (0)

/* Add ctpr3 to clobbers to explain to lcc that this
 * GNU asm does a return. */
#define E2K_DONE \
do { \
	asm volatile ("{nop 3} {done}" ::: "ctpr3"); \
} while (0)

#define E2K_EMPTY_CMD(input...) \
do { \
	asm volatile ("{nop}" :: input); \
} while (0)


/*
 * E2K_GOTO macro is I hope temporary 'thing' that will remain
 * until we've got some support of e2k-gcc compiler here.
 */
#define E2K_GOTO(label) \
({                      \
        asm volatile (  \
		"{ ibranch \t" #label " }"	\
	);					\
})

#if __LCC__ >= 120

#define GET_USER_ASM(x, addr, fmt, __ret_gu)			\
	asm ("1:{ld" #fmt "[%2 + 0], %1}\n"			\
	     "2:\n"						\
	     ".section .fixup,\"ax\"\n"				\
	     "3:{adds 0, %3, %0\n"				\
	     "  ibranch 2b}\n"					\
	     ".previous\n"					\
	     ".section __ex_table,\"a\"\n"			\
	     ".dword 1b, 3b\n"					\
	     ".previous\n"					\
	     : "+r" (__ret_gu), "=r"(x)				\
	     : "m" (*addr), "i" (-EFAULT))

#define PUT_USER_ASM(x, ptr, fmt, retval)			\
	asm ("1:{st" #fmt " [%2 + 0], %1}\n"			\
	     "2:\n"						\
	     ".section .fixup,\"ax\"\n"				\
	     "3:\n{adds 0, %3, %0\n"				\
	     "  ibranch 2b}\n"					\
	     ".previous\n"					\
	     ".section __ex_table,\"a\"\n"			\
	     ".dword 1b, 3b\n"					\
	     ".previous\n"					\
	     : "+r" (retval)					\
	     : "r" (x), "m" (*ptr), "i" (-EFAULT))

#else	/*  __LCC__ < 120 */

/*
 * asm is used because of posible restrict attribute of ptr
 * to avoid this problem we must use ASM
 */

#define PUT_USER(x, ptr, size, retval)				\
{								\
	  switch (size) {					\
	     /* *((u_int8_t *)ptr) = (u_int8_t)(x); */			\
	  case 1:						\
		  asm volatile ("{stb [%0 + 0],[%1]}"		\
				: : "r" (ptr), "r" (x)); break;	\
	     /* *((u_int16_t *)ptr) = (u_int16_t)(x); */			\
	  case 2:						\
		 asm volatile ("{sth [%0 + 0],[%1]}"		\
				: : "r" (ptr), "r" (x)); break;	\
	     /* *((u_int32_t *)ptr) = (u_int32_t)(x); */			\
	  case 4:						\
		  asm volatile ("{stw [%0 + 0],[%1]}"		\
				: : "r" (ptr), "r" (x)); break;	\
	     /* *((u_int64_t *)ptr) = (u_int64_t)(x); */			\
	  case 8:						\
		 asm volatile ("{std [%0 + 0],[%1]}"		\
				: : "r" (ptr), "r" (x)); break;	\
	  default:						\
		   retval = -EFAULT;		break;		\
	}							\
}

#endif /* __LCC__ >= 120 */

#ifndef	__ASSEMBLER__

#define E2K_PARALLEL_WRITE(addr1, val1, addr2, val2) \
{                                                                       \
	asm volatile ("{\n\t"                                               \
		      "  std 0x0, %2, %4\n\t"                               \
		      "  std 0x0, %3, %5\n\t"                               \
		      "}"                                                   \
		      : "=m" (*(addr1)), "=m" (*(addr2))                    \
		      : "r" (addr1), "r" (addr2), "r" (val1), "r" (val2));  \
}

/*
 *                     Macroses to construct alternative return point from trap
 */

#define STICK_ON_REG(reg)	asm( #reg )

/* Global labels support has been dropped in lcc 1.20 */
#if __LCC__ >= 120
# define GET_LBL_ADDR(name, local_name, where)	\
			_Pragma("no_asm_inline") \
			asm ("movtd [" local_name "], %0" : "=r" (where))
# define TRAP_RETURN_LABEL(name, local_name)	\
({ \
	unsigned long __trap_return_label; \
	_Pragma("no_asm_inline") \
	asm volatile    (local_name : "=r" (__trap_return_label)); \
})
#else
# define GET_LBL_ADDR(name, local_name, where)	\
			asm ("movtd [$." name "], %0" : "=r" (where))
# define TRAP_RETURN_LABEL(name, local_name)	\
({ \
	unsigned long __trap_return_label; \
	asm volatile    ("$." name ":" : "=r" (__trap_return_label)); \
})
#endif

#define E2K_JUMP_ABSOLUTE(func) \
do { \
	asm volatile ("{\n" \
		      "movtd %[_func], %%ctpr1\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      :: [_func] "ir" (func) \
		      : "ctpr1"); \
} while (0)

#define E2K_JUMP(func)	E2K_JUMP_WITH_ARGUMENTS(func, 0)

#define E2K_JUMP_WITH_ARGUMENTS(func, num_args, ...) \
	__E2K_JUMP_WITH_ARGUMENTS_##num_args(func ,##__VA_ARGS__)

#define __E2K_JUMP_WITH_ARGUMENTS_0(func) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      : : : "ctpr1"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_1(func, arg1) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      : \
		      : "ri" (arg1) \
		      : "ctpr1", "r0"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_2(func, arg1, arg2) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2) \
		      : "ctpr1", "r0", "r1"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_3(func, arg1, arg2, arg3) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "addd  %2, 0, %%dr2\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2), "ri" (arg3) \
		      : "ctpr1", "r0", "r1", "r2"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_4(func, arg1, arg2, arg3, arg4) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "addd  %2, 0, %%dr2\n" \
		      "addd  %3, 0, %%dr3\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2), "ri" (arg3), "ri" (arg4) \
		      : "ctpr1", "r0", "r1", "r2", "r3"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_5(func, arg1, arg2, arg3, arg4, arg5) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "addd  %2, 0, %%dr2\n" \
		      "addd  %3, 0, %%dr3\n" \
		      "addd  %4, 0, %%dr4\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2), "ri" (arg3), "ri" (arg4), \
		        "ri" (arg5) \
		      : "ctpr1", "r0", "r1", "r2", "r3", "r4"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_6(func, arg1, arg2, arg3, arg4, arg5, arg6) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "addd  %2, 0, %%dr2\n" \
		      "addd  %3, 0, %%dr3\n" \
		      "addd  %4, 0, %%dr4\n" \
		      "addd  %5, 0, %%dr5\n" \
		      "}\n" \
		      "ct %%ctpr1\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2), "ri" (arg3), "ri" (arg4), \
		        "ri" (arg5), "ri" (arg6) \
		      : "ctpr1", "r0", "r1", "r2", "r3", "r4", "r5"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_7(func, arg1, arg2, arg3, arg4, \
				    arg5, arg6, arg7) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "addd  %2, 0, %%dr2\n" \
		      "addd  %3, 0, %%dr3\n" \
		      "addd  %4, 0, %%dr4\n" \
		      "addd  %5, 0, %%dr5\n" \
		      "}\n" \
		      "{\n" \
		      "addd  %6, 0, %%dr6\n" \
		      "ct %%ctpr1\n" \
		      "}\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2), "ri" (arg3), "ri" (arg4), \
		        "ri" (arg5), "ri" (arg6), "ri" (arg7) \
		      : "ctpr1", "r0", "r1", "r2", "r3", "r4", "r5", "r6"); \
} while (0)

#define __E2K_JUMP_WITH_ARGUMENTS_8(func, arg1, arg2, arg3, arg4, \
				    arg5, arg6, arg7, arg8) \
do { \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "addd  %2, 0, %%dr2\n" \
		      "addd  %3, 0, %%dr3\n" \
		      "addd  %4, 0, %%dr4\n" \
		      "addd  %5, 0, %%dr5\n" \
		      "}\n" \
		      "{\n" \
		      "addd  %6, 0, %%dr6\n" \
		      "addd  %7, 0, %%dr7\n" \
		      "ct %%ctpr1\n" \
		      "}\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2), "ri" (arg3), "ri" (arg4), \
		        "ri" (arg5), "ri" (arg6), "ri" (arg7), "ri" (arg8) \
		      : "ctpr1", "r0", "r1", "r2", "r3", "r4", "r5", "r6", \
		        "r7"); \
} while (0)

#define E2K_CALL_KEEP_ARGUMENTS_3(func, arg1, arg2, arg3) \
do { \
	(void) &func; \
	asm volatile ("{\n" \
		      "disp %%ctpr1, " #func "\n" \
		      "addd  %0, 0, %%dr0\n" \
		      "addd  %1, 0, %%dr1\n" \
		      "addd  %2, 0, %%dr2\n" \
		      "}\n" \
		      "call %%ctpr1, wbs=%#\n" \
		      : \
		      : "ri" (arg1), "ri" (arg2), "ri" (arg3) \
		      : E2K_CALL_CLOBBERS); \
} while (0)

/* Important: delay after FPU reading is 9 cycles for 0 cluster
 * and 11 for 1 cluster, thus the NOPs. */
#define E2K_GETCONTEXT(fpcr, fpsr, pfpfr, pcsp_lo, pcsp_hi) \
do { \
	u_int64_t __pcshtp; \
	asm volatile ("rrs %%fpcr, %0\n" \
		      "rrs %%fpsr, %1\n" \
		      "rrs %%pfpfr, %2\n" \
		      "rrd %%pcshtp, %5\n" \
		      "rrd %%pcsp.lo, %3\n" \
		      "{rrd %%pcsp.hi, %4\n" \
		      "shld %5, 53, %5}\n" \
		      "sard %5, 53, %5\n" \
		      "{addd %4, %5, %4\n" \
		      "nop 5}\n" \
		      : "=r" (fpcr), "=r" (fpsr), "=r" (pfpfr), \
			"=r" (pcsp_lo), "=r" (pcsp_hi), "=r" (__pcshtp) \
		      : ); \
} while (0)

/* Important: delay after FPU reading is 9 cycles for 0 cluster
 * and 11 for 1 cluster, thus the NOPs. */
#define E2K_GET_FPU(fpcr, fpsr, pfpfr) \
do { \
	asm volatile ("rrs %%fpcr, %0\n" \
		      "rrs %%fpsr, %1\n" \
		      "{rrs %%pfpfr, %2\n" \
		      "nop 5}\n" \
		      "{nop 4}\n" \
		      : "=r" (fpcr), "=r" (fpsr), "=r" (pfpfr) \
		      : ); \
} while (0)

#define E2K_CLEAR_RF_112() \
do { \
	asm volatile ( \
		"{\n" \
		"nop 3\n" \
		"disp %%ctpr1, 1f\n" \
		"setwd wsz=112\n" \
		"setbn rbs=0, rsz=62, rcur=0\n" \
		"rwd 21UL | (1UL << 37), %%lsr\n" \
		"}\n" \
		"{\n" \
		"disp %%ctpr2, 2f\n" \
		"}\n" \
		"1:" \
		"{\n" \
		"loop_mode\n" \
		"addd 0, 0, %%db[0]\n" \
		"addd 0, 0, %%db[1]\n" \
		"addd 0, 0, %%db[42]\n" \
		"addd 0, 0, %%db[43]\n" \
		"addd 0, 0, %%db[84]\n" \
		"addd 0, 0, %%db[85]\n" \
		"alc alcf = 1, alct = 1\n" \
		"abn abnf = 1, abnt = 1\n" \
		"ct %%ctpr1 ? %%NOT_LOOP_END\n" \
		"}\n" \
		"{\n" \
		"nop 4\n" \
		"setbn rbs=63, rsz=48, rcur=0\n" \
		"rwd 16UL | (1UL << 37), %%lsr\n" \
		"}\n" \
		"2:" \
		"{\n" \
		"loop_mode\n" \
		"addd 0, 0, %%db[0]\n" \
		"addd 0, 0, %%db[1]\n" \
		"addd 0, 0, %%db[32]\n" \
		"addd 0, 0, %%db[33]\n" \
		"addd 0, 0, %%db[64]\n" \
		"addd 0, 0, %%db[65]\n" \
		"alc alcf = 1, alct = 1\n" \
		"abn abnf = 1, abnt = 1\n" \
		"ct %%ctpr2 ? %%NOT_LOOP_END\n" \
		"}\n" \
		"{\n" \
		"addd 0, 0, %%db[64]\n" \
		"addd 0, 0, %%db[65]\n" \
		"}\n" \
		::: "ctpr1", "ctpr2"); \
} while (0)

#define	E2K_CLEAR_CTPRS()			\
do {						\
	uint64_t	reg;			\
	asm volatile (				\
		"{\n"				\
		"puttagd 0, 5, %0\n"		\
		"movtd,s %0, %%ctpr1\n"		\
		"}\n"				\
		"{\n"				\
		"movtd,s %0, %%ctpr2\n"		\
		"}\n"				\
		"{\n"				\
		"movtd,s %0, %%ctpr3\n"		\
		"}\n"				\
		: "=r" (reg)			\
		:				\
		: "ctpr1", "ctpr2", "ctpr3");	\
} while (0)

#define E2K_SET_LSR_LSR1_ILCR_ILCR1(_lsr, _lsr1, _ilcr, _ilcr1) \
do { \
	_Pragma("no_asm_inline") \
	asm volatile ("{\n" \
		      "rwd %[lsr], %%lsr\n" \
		      "addd %[lsr1], 0, %%db[1]\n" \
		      "addd %[ilcr1], 0, %%db[3]\n" \
		      "}\n" \
		      /* rwd %db[1], %%lsr1 */ \
		      ".word 0x04100011\n" \
		      ".word 0x3dc001c3\n" \
		      ".word 0x01c00000\n" \
		      ".word 0x00000000\n" \
		      "{\n" \
		      "rwd %[ilcr], %%ilcr\n" \
		      "}\n" \
		      /* rwd %db[3], %%ilcr1 */ \
		      ".word 0x04100011\n" \
		      ".word 0x3dc003c7\n" \
		      ".word 0x01c00000\n" \
		      ".word 0x00000000\n" \
		      :: [lsr] "r" (_lsr), [lsr1] "r" (_lsr1), \
		         [ilcr] "r" (_ilcr), [ilcr1] "r" (_ilcr1) \
		      : "memory", "b[1]", "b[3]"); \
} while (0)

#define cpu_relax() __asm__ __volatile__("{nop 7}" ::: IRQ_BARRIER_CLOBBERS)

static inline void prefetch(const void *ptr)
{
	/* Cannot use __builtin_prefetch() here since ptr could be NULL */
	E2K_PREFETCH_L2_SPEC(ptr);
}

static inline void prefetchw(const void *ptr)
{
	__builtin_prefetch(ptr);
}

/*  Use L2 cache line size since we are prefetching to L2 */
#define PREFETCH_STRIDE 64

static inline void prefetchw_range(void *addr, unsigned long len)
{
	unsigned long i, rem, prefetched;

	if (__builtin_constant_p(len) && len < 24 * PREFETCH_STRIDE) {
		if (len > 0)
			prefetchw(addr);
		if (len > PREFETCH_STRIDE)
			prefetchw(addr + PREFETCH_STRIDE);
		if (len > 2 * PREFETCH_STRIDE)
			prefetchw(addr + 2 * PREFETCH_STRIDE);
		if (len > 3 * PREFETCH_STRIDE)
			prefetchw(addr + 3 * PREFETCH_STRIDE);
		if (len > 4 * PREFETCH_STRIDE)
			prefetchw(addr + 4 * PREFETCH_STRIDE);
		if (len > 5 * PREFETCH_STRIDE)
			prefetchw(addr + 5 * PREFETCH_STRIDE);
		if (len > 6 * PREFETCH_STRIDE)
			prefetchw(addr + 6 * PREFETCH_STRIDE);
		if (len > 7 * PREFETCH_STRIDE)
			prefetchw(addr + 7 * PREFETCH_STRIDE);
		if (len > 8 * PREFETCH_STRIDE)
			prefetchw(addr + 8 * PREFETCH_STRIDE);
		if (len > 9 * PREFETCH_STRIDE)
			prefetchw(addr + 9 * PREFETCH_STRIDE);
		if (len > 10 * PREFETCH_STRIDE)
			prefetchw(addr + 10 * PREFETCH_STRIDE);
		if (len > 11 * PREFETCH_STRIDE)
			prefetchw(addr + 11 * PREFETCH_STRIDE);
		if (len > 12 * PREFETCH_STRIDE)
			prefetchw(addr + 12 * PREFETCH_STRIDE);
		if (len > 13 * PREFETCH_STRIDE)
			prefetchw(addr + 13 * PREFETCH_STRIDE);
		if (len > 14 * PREFETCH_STRIDE)
			prefetchw(addr + 14 * PREFETCH_STRIDE);
		if (len > 15 * PREFETCH_STRIDE)
			prefetchw(addr + 15 * PREFETCH_STRIDE);
		if (len > 16 * PREFETCH_STRIDE)
			prefetchw(addr + 16 * PREFETCH_STRIDE);
		if (len > 17 * PREFETCH_STRIDE)
			prefetchw(addr + 17 * PREFETCH_STRIDE);
		if (len > 18 * PREFETCH_STRIDE)
			prefetchw(addr + 18 * PREFETCH_STRIDE);
		if (len > 19 * PREFETCH_STRIDE)
			prefetchw(addr + 19 * PREFETCH_STRIDE);
		if (len > 20 * PREFETCH_STRIDE)
			prefetchw(addr + 20 * PREFETCH_STRIDE);
		if (len > 21 * PREFETCH_STRIDE)
			prefetchw(addr + 21 * PREFETCH_STRIDE);
		if (len > 22 * PREFETCH_STRIDE)
			prefetchw(addr + 22 * PREFETCH_STRIDE);
		if (len > 23 * PREFETCH_STRIDE)
			prefetchw(addr + 23 * PREFETCH_STRIDE);

		return;
	}

	for (i = 0; i <= len - 4 * PREFETCH_STRIDE; i += 4 * PREFETCH_STRIDE) {
		prefetchw(addr + i);
		prefetchw(addr + i + PREFETCH_STRIDE);
		prefetchw(addr + i + 2 * PREFETCH_STRIDE);
		prefetchw(addr + i + 3 * PREFETCH_STRIDE);
	}

	rem = len % (4 * PREFETCH_STRIDE);
	prefetched = len / (4 * PREFETCH_STRIDE);

	if (rem > 0)
		prefetchw(addr + prefetched);
	if (rem > PREFETCH_STRIDE)
		prefetchw(addr + prefetched + PREFETCH_STRIDE);
	if (rem > 2 * PREFETCH_STRIDE)
		prefetchw(addr + prefetched + 2 * PREFETCH_STRIDE);
	if (rem > 3 * PREFETCH_STRIDE)
		prefetchw(addr + prefetched + 3 * PREFETCH_STRIDE);
}

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ __here; __here: &&__here; })

#define E2K_READ_STACK_REGS(addr, pcsp_hi, psp_hi, pcshtp, pshtp) \
do { \
	char val; \
	asm volatile ("1:\n" \
		      "ldb,0 [ %[__addr] + 0 ] 0x7, %[val]\n" \
		      "rrd %%pcshtp, %[__pcshtp]\n" \
		      "rrd %%pcsp.hi, %[__pcsp_hi]\n" \
		      "rrd %%pshtp, %[__pshtp]\n" \
		      "rrd %%psp.hi, %[__psp_hi]\n" \
		      "{\n" \
		      "stb,2 [ %[__addr] + 0 ] 0x2, %[val]\n" \
		      "ibranch 1b ? %%MLOCK\n" \
		      "}\n" \
		      : [val] "=&r" (val), \
			[__pcsp_hi] "=&r" (pcsp_hi), \
			[__psp_hi] "=&r" (psp_hi), \
			[__pcshtp] "=&r" (pcshtp), \
			[__pshtp] "=&r" (pshtp) \
		      : [__addr] "r" (addr)); \
} while (0)

#define E2K_RETURN32(ret) \
do { \
	asm volatile (  "{\n" \
			"return %%ctpr3\n" \
			"adds %[_ret], 0, %%dr0\n" \
			"}\n" \
			"{\n" \
			"ct %%ctpr3\n" \
			"}\n" \
			:: [_ret] "ir" (ret) \
			: "ctpr3"); \
} while (0)

#define E2K_SETSFT()	asm volatile ("{setsft}" ::)

#endif /* __ASSEMBLER__ */

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
