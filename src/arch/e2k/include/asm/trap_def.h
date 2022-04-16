/*
 *
 * Copyright (C) 2001 MCST
 *
 * Defenition of traps handling routines.
 */

#ifndef _E2K_TRAP_DEF_H
#define _E2K_TRAP_DEF_H

//#include <linux/types.h>

#define GET_NR_TIRS(tir_hi)		((tir_hi >> 56) & 0xff)
#define	GET_CLEAR_TIR_HI(tir_no)	(((tir_no) & 0xffUL) << 56)
#define	GET_CLEAR_TIR_LO(tir_no)	0UL

/* get aa field of tir_hi register */
#define GET_AA_TIRS(tir_hi)		((tir_hi >> 52) & 0x0f)
#define	SET_AA_TIRS(tir_hi, aa_field)	((tir_hi) | (((aa_field) & 0x0f) << 52))
/* get IP field of tir_lo register */
#define GET_IP_TIRS(tir_lo)		((tir_lo) & E2K_VA_MASK)
/* get IP field of cr0_hi register */
#define GET_IP_CR0_HI(cr0_hi)		((cr0_hi).CR0_hi_ip << E2K_ALIGN_INS)

/*
 * Trap Info Register: the numbers of exceptions
 */

#define	exc_illegal_opcode_num		0
#define	exc_priv_action_num		1
#define	exc_fp_disabled_num		2
#define	exc_fp_stack_u_num		3
#define	exc_d_interrupt_num		4
#define	exc_diag_ct_cond_num		5
#define	exc_diag_instr_addr_num		6
#define	exc_illegal_instr_addr_num	7
#define	exc_instr_debug_num		8
#define	exc_window_bounds_num		9
#define	exc_user_stack_bounds_num	10
#define	exc_proc_stack_bounds_num	11
#define	exc_chain_stack_bounds_num	12
#define	exc_fp_stack_o_num		13
#define	exc_diag_cond_num		14
#define	exc_diag_operand_num		15
#define	exc_illegal_operand_num		16
#define	exc_array_bounds_num		17
#define	exc_access_rights_num		18
#define	exc_addr_not_aligned_num	19
#define	exc_instr_page_miss_num		20
#define	exc_instr_page_prot_num		21
#define	exc_ainstr_page_miss_num	22
#define	exc_ainstr_page_prot_num	23
#define	exc_last_wish_num		24
#define	exc_base_not_aligned_num	25
#define	exc_software_trap_num		26

#define	exc_data_debug_num		28
#define	exc_data_page_num		29

#define	core_dump_num			30

#define	exc_recovery_point_num		31
#define	exc_interrupt_num		32
#define	exc_nm_interrupt_num		33
#define	exc_div_num			34
#define	exc_fp_num			35
#define	exc_mem_lock_num		36
#define	exc_mem_lock_as_num		37
#define	exc_data_error_num		38
#define	exc_mem_error_out_cpu_num	38
#define	exc_mem_error_MAU_num		39
#define	exc_mem_error_L2_num		40
#define	exc_mem_error_L1_35_num		41
#define	exc_mem_error_L1_02_num		42
#define	exc_mem_error_ICACHE_num	43

#define	exc_max_num			43

#define	exc_mova_ch_0_num		52	/* [52] TIR.aa.[0] */
#define	exc_mova_ch_1_num		53	/* [53] TIR.aa.[1] */
#define	exc_mova_ch_2_num		54	/* [54] TIR.aa.[2] */
#define	exc_mova_ch_3_num		55	/* [55] TIR.aa.[3] */

#define	masked_hw_stack_bounds_num	60	/* hardware stacks bounds */
						/* trap is occured but masked */

/*
 * Trap Info Register: the bit mask of exceptions
 */

#define	exc_illegal_opcode_mask		(1UL << exc_illegal_opcode_num)
#define	exc_priv_action_mask		(1UL << exc_priv_action_num)
#define	exc_fp_disabled_mask		(1UL << exc_fp_disabled_num)
#define	exc_fp_stack_u_mask		(1UL << exc_fp_stack_u_num)
#define	exc_d_interrupt_mask		(1UL << exc_d_interrupt_num)
#define	exc_diag_ct_cond_mask		(1UL << exc_diag_ct_cond_num)
#define	exc_diag_instr_addr_mask	(1UL << exc_diag_instr_addr_num)
#define	exc_illegal_instr_addr_mask	(1UL << exc_illegal_instr_addr_num)
#define	exc_instr_debug_mask		(1UL << exc_instr_debug_num)
#define	exc_window_bounds_mask		(1UL << exc_window_bounds_num)
#define	exc_user_stack_bounds_mask	(1UL << exc_user_stack_bounds_num)
#define	exc_proc_stack_bounds_mask	(1UL << exc_proc_stack_bounds_num)
#define	exc_chain_stack_bounds_mask	(1UL << exc_chain_stack_bounds_num)
#define	exc_fp_stack_o_mask		(1UL << exc_fp_stack_o_num)
#define	exc_diag_cond_mask		(1UL << exc_diag_cond_num)
#define	exc_diag_operand_mask		(1UL << exc_diag_operand_num)
#define	exc_illegal_operand_mask	(1UL << exc_illegal_operand_num)
#define	exc_array_bounds_mask		(1UL << exc_array_bounds_num)
#define	exc_access_rights_mask		(1UL << exc_access_rights_num)
#define	exc_addr_not_aligned_mask	(1UL << exc_addr_not_aligned_num)
#define	exc_instr_page_miss_mask	(1UL << exc_instr_page_miss_num)
#define	exc_instr_page_prot_mask	(1UL << exc_instr_page_prot_num)
#define	exc_ainstr_page_miss_mask	(1UL << exc_ainstr_page_miss_num)
#define	exc_ainstr_page_prot_mask	(1UL << exc_ainstr_page_prot_num)
#define	exc_last_wish_mask		(1UL << exc_last_wish_num)
#define	exc_base_not_aligned_mask	(1UL << exc_base_not_aligned_num)
#define	exc_software_trap_mask		(1UL << exc_software_trap_num)

#define	exc_data_debug_mask		(1UL << exc_data_debug_num)
#define	exc_data_page_mask		(1UL << exc_data_page_num)

#define	exc_recovery_point_mask		(1UL << exc_recovery_point_num)
#define	exc_interrupt_mask		(1UL << exc_interrupt_num)
#define	exc_nm_interrupt_mask		(1UL << exc_nm_interrupt_num)
#define	exc_div_mask			(1UL << exc_div_num)
#define	exc_fp_mask			(1UL << exc_fp_num)
#define	exc_mem_lock_mask		(1UL << exc_mem_lock_num)
#define	exc_mem_lock_as_mask		(1UL << exc_mem_lock_as_num)
#define	exc_data_error_mask		(1UL << exc_data_error_num)
#define	exc_mem_error_out_cpu_mask	(1UL << exc_mem_error_out_cpu_num)
#define	exc_mem_error_MAU_mask		(1UL << exc_mem_error_MAU_num)
#define	exc_mem_error_L2_mask		(1UL << exc_mem_error_L2_num)
#define	exc_mem_error_L1_35_mask	(1UL << exc_mem_error_L1_35_num)
#define	exc_mem_error_L1_02_mask	(1UL << exc_mem_error_L1_02_num)
#define	exc_mem_error_ICACHE_mask	(1UL << exc_mem_error_ICACHE_num)
#define	exc_mem_error_mask		(exc_mem_error_out_cpu_mask |	\
					exc_mem_error_MAU_mask |	\
					exc_mem_error_L2_mask |		\
					exc_mem_error_L1_35_mask |	\
					exc_mem_error_L1_02_mask |	\
					exc_mem_error_ICACHE_mask)

#define	exc_mova_ch_0_mask		(1UL << exc_mova_ch_0_num)
#define	exc_mova_ch_1_mask		(1UL << exc_mova_ch_1_num)
#define	exc_mova_ch_2_mask		(1UL << exc_mova_ch_2_num)
#define	exc_mova_ch_3_mask		(1UL << exc_mova_ch_3_num)

#define	exc_all_mask			((1UL << (exc_max_num + 1)) - 1UL)
#define aau_exc_mask			(exc_mova_ch_0_mask | \
					exc_mova_ch_1_mask | \
					exc_mova_ch_2_mask | \
					exc_mova_ch_3_mask)

#define	core_dump_mask			(1UL << core_dump_num)
#define	masked_hw_stack_bounds_mask	(1UL << masked_hw_stack_bounds_num)

#define	sync_exc_mask			(exc_illegal_opcode_mask |	\
					exc_priv_action_mask |		\
					exc_fp_disabled_mask |		\
					exc_fp_stack_u_mask |		\
					exc_diag_ct_cond_mask |		\
					exc_diag_instr_addr_mask |	\
					exc_illegal_instr_addr_mask |	\
					exc_window_bounds_mask |	\
					exc_user_stack_bounds_mask |	\
					exc_fp_stack_o_mask |		\
					exc_diag_cond_mask |		\
					exc_diag_operand_mask |		\
					exc_illegal_operand_mask |	\
					exc_array_bounds_mask |		\
					exc_access_rights_mask |	\
					exc_addr_not_aligned_mask |	\
					exc_instr_page_miss_mask |	\
					exc_instr_page_prot_mask |	\
					exc_base_not_aligned_mask |	\
					exc_software_trap_mask)

#define	async_exc_mask			(exc_proc_stack_bounds_mask |	\
					exc_chain_stack_bounds_mask |	\
					exc_instr_debug_mask |		\
					exc_ainstr_page_miss_mask |	\
					exc_ainstr_page_prot_mask |	\
					exc_interrupt_mask |		\
					exc_nm_interrupt_mask |		\
					exc_mem_lock_as_mask |		\
					exc_data_error_mask |		\
					exc_mem_error_mask)

#define	defer_exc_mask			(exc_data_page_mask |		\
					exc_mem_lock_mask |		\
					exc_d_interrupt_mask |		\
					exc_last_wish_mask)

/* Mask of non-maskable interrupts. "exc_data_debug" and "exc_instr_debug"
 * actually can be either maskable or non-maskable depending on the watched
 * event, but we assume the worst case (non-maskable). */
#define non_maskable_exc_mask		(exc_nm_interrupt_mask | \
					exc_data_debug_mask | \
					exc_instr_debug_mask | \
					exc_mem_lock_as_mask)

#define have_tc_exc_mask		(exc_data_page_mask | \
					exc_mem_lock_mask)

#define fp_es	(1UL << 7)	/* - error summary status; es set if anyone */
				/*   of unmasked exception flags is set; */

#define fp_pe	(1UL << 5)	/* - precision exception flag; */
#define fp_ue	(1UL << 4)	/* - underflow exception flag; */
#define fp_oe	(1UL << 3)	/* - overflow exception flag; */
#define fp_ze	(1UL << 2)	/* - divide by zero exception flag; */
#define fp_de	(1UL << 1)	/* - denormalized operand exception flag; */
#define fp_ie	(1UL << 0)	/* - invalid operation exception flag; */

#ifndef	__ASSEMBLY__
/*
 * do_page_fault() return values
 */
enum pf_ret {
	/* Could not handle fault, must return to handle signals */
	PFR_SIGPENDING = 1,
	/* The page fault was handled */
	PFR_SUCCESS,
	/* In some cases kernel addresses can be in Trap Cellar if VLIW command
	 * consisted of a several load/store operations and one of them caused
	 * page fault trap */
	PFR_KERNEL_ADDRESS,
	/* Do not handle speculative access */
	PFR_IGNORE,
	/* Controlled access from kernel to user memory */
	PFR_CONTROLLED_ACCESS,
	/* needs to change SAP to AP for multi_threading of protected mode */
	PFR_AP_THREAD_READ,
	/* trap on paravirtualized guest kernel and is handled by host: */
	/* such result means the trap was handled by hypervisor and it need */
	/* recover faulted operation */
	PFR_KVM_KERNEL_ADDRESS,
};
#endif	/* ! __ASSEMBLY__ */

/*
 * Common system calls (trap table entries numbers)
 */
#define LINUX_SYSCALL32_TRAPNUM		1	/* Use E2K trap entry #1 */
#define LINUX_SYSCALL64_TRAPNUM		3	/* Use E2K trap entry #3 */
#define	LINUX_SYSCALL_TRAPNUM_OLD	4	/* Deprecated */
#define LINUX_FAST_SYSCALL32_TRAPNUM	5
#define LINUX_FAST_SYSCALL64_TRAPNUM	6
#define LINUX_FAST_SYSCALL128_TRAPNUM	7

/*
 * Hypercalls
 */
#define LINUX_HCALL_GENERIC_TRAPNUM	0 /* guest hardware hypercalls */
#define LINUX_HCALL_LIGHT_TRAPNUM	1 /* guest light hardware hypercalls */

/*
 * Definition of ttable entry number used for protected system calls.
 * This is under agreement with protected mode compiler/plib team.
 */
#define	PMODE_SYSCALL_TRAPNUM	8
#define	PMODE_NEW_SYSCALL_TRAPNUM	10

#define GENERIC_HYPERCALL_TRAPNUM	16 /* guest software hypercalls */
#define LIGHT_HYPERCALL_TRAPNUM		17 /* guest light software hypercalls */

#define	HYPERCALLS_TRAPS_MASK		((1U << GENERIC_HYPERCALL_TRAPNUM) | \
					(1U << LIGHT_HYPERCALL_TRAPNUM))

/*
 * One trap table entry byte size
 */
#define	LINUX_SYSCALL_ENTRY_SIZE	0x800	/* one entry max size is */
						/* PAGE_SIZE / 2 */

#endif	/* _E2K_TRAP_DEF_H */
