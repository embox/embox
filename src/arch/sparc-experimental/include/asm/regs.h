/**
 * @file
 * @brief Defines local registers usage layout for the trap context.
 *
 * @date 29.11.2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_REG_ALIAS_H_
#define SPARC_REG_ALIAS_H_

#ifdef __ASSEMBLER__

/**
 * Trap time PSR. Written each time when trap occurs. In general this register
 * should not be touched until @link SAVE_ALL @endlink has not been executed.
 * It's value is not touched by trap setup routines so usually there is no need
 * to reread PSR after returning from @c SAVE_ALL .
 */
#define t_psr      l0

/**
 * PC to return from trap. Filled by the hardware. This register should not be
 * touched until @link SAVE_ALL @endlink has not been executed.
 */
#define t_pc       l1

/**
 * nPC to return from trap. Filled by the hardware. This register should not be
 * touched until @link SAVE_ALL @endlink has not been executed.
 */
#define t_npc      l2

/**
 * Current WIM. Initialized either by trap table entry (window
 * overflow/underflow traps) or when branching to trap setup routines with
 * @link SAVE_ALL @endlink / @link RESTORE_ALL @endlink macros.
 */
#define t_wim      l3

/**
 * The mask of the trap time window (1 << CWP). Chiefly used in trap setup and
 * windows-related routines.
 */
#define t_twinmask l4

/**
 * This register does not define concrete semantics. Usually trap setup code
 * defines an alias for this name suitable for the particular context.
 * This register can be used by regular code as general purpose register but
 * considering that it is volatile across @link SAVE_ALL @endlink macro call.
 */
#define temp       l5

/**
 * General purpose local register. Unlike @link temp @endlink it can be safely
 * used through @link SAVE_ALL @endlink call.
 */
#define local      l6

/**
 * Local return address used instead of %o7 when window overflow check
 * has not been performed yet.
 */
#define t_retpc    l7

#define WRITE_PAUSE  nop; nop; nop;

#endif /* __ASSEMBLER__ */

#endif /* SPARC_REG_ALIAS_H_ */
