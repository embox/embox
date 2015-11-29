/** 
 * @file	mem_barriers.c
 * @brief	Memory barriers or data cache invalidation/flushing for ARM processors
 *
 * @description Memory barriers or data cache invalidation/flushing may be
 * required around mailbox accesses, details on this needs to be added to this
 * page (or the page for the particular mailbox/channel that requires it) by
 * someone who knows the details.
 *
 * The following instructions are taken from
 * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0360f/I1014942.html. Any
 * unneeded register can be used in the following example instead of r3.
 *
 * mov r3, #0                      # The read register Should Be Zero before the call
 * mcr p15, 0, r3, c7, c6, 0       # Invalidate Entire Data Cache
 * mcr p15, 0, r3, c7, c10, 0      # Clean Entire Data Cache
 * mcr p15, 0, r3, c7, c14, 0      # Clean and Invalidate Entire Data Cache
 * mcr p15, 0, r3, c7, c10, 4      # Data Synchronization Barrier
 * mcr p15, 0, r3, c7, c10, 5      # Data Memory Barrier
 *
 * @author	Michele Di Giorgio
 * @date	23.11.2015
 */

/**
 * This instruction ensures all data memory accesses are completed before the
 * next instruction.
 */
static inline void data_mem_barrier(void) {
	__asm__ __volatile__("mcr p15, 0, %0, c7, c10, 5"
			: : "r" (0) : "memory");
}
