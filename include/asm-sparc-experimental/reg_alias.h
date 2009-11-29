/**
 * @file reg_alias.h
 *
 * @brief Defines common register aliases used in a trap context.
 *
 * @date 29.11.2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_REG_ALIAS_H_
#define SPARC_REG_ALIAS_H_

#ifdef __ASSEMBLER__

/**
 * Trap time PSR. Written each time when trap occurs.
 * @note In general this register should not be touched
 * until @link SAVE_ALL @endlink has not been executed.
 *
 * @sa trap_table.h
 */
#define t_psr      l0

/**
 * PC to return from trap. Filled by the hardware.
 * @note In general this register should not be touched
 * until @link SAVE_ALL @endlink has not been executed.
 */
#define t_pc       l1

/**
 * PC to return from trap. Filled by the hardware.
 * @note In general this register should not be touched
 * until @link SAVE_ALL @endlink has not been executed.
 */
#define t_npc      l2

/** Current WIM. */
#define t_wim      l3

/**
 * The mask of the trap time window (1 << CWP).
 */
#define t_twinmask l4

/**
 * The mask of the last user window (if any).
 */
#define t_uwinmask l5

/**
 * Kernel stack pointer. Used in the trap setup routines.
 */
#define t_ksp      l6

/**
 * Local return address used instead of o7 when window overflow check
 * has not been performed yet.
 */
#define t_retpc    l7

#endif /* __ASSEMBLER__ */

#endif /* SPARC_REG_ALIAS_H_ */
