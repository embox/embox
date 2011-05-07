/**
 * @file
 * @brief Describes SPARC v8 WIM (Window Invalid Mask) management policy.
 * @details Here is listed only an introduction to the policy. The particular
 * cases are discussed in the corresponding files which contain core trap setup
 * and WOF/WUF handling routines.
 *
 * We do a small trick based on a nonstandard usage of the @em WIM register.
 * The main advantage of this method is that it allows to quickly determine
 * a presence of user windows on the CPU.
 *
 * More definitely, we encode in the @em WIM not only the invalid window marker
 * but also the mask of the last user window (if any) being on the CPU.
 * Thus, one may distinguish @em WIM bits to @b primary (stands for traditional
 * invalid window mask) and @b secondary (represents the last user window)
 * markers.
 *
 * Some restrictions and assumptions needed for this method to work properly:
 *   @n 1) Kernel code does not "over-restores" (particularly, it should
 * not perform "<em>RESTORE</em> - <em>SAVE</em>" combinations).
 *   @n 2) At any moment each kernel window has proper stack pointer.
 *   @n 3) CPU has got 3 or more register windows. Because of this fact it is
 * guaranteed that bitwise @em AND of next and previous windows masks is always
 * zero. This means that <em>OR</em>ing, <em>XOR</em>ing, <em>ADD</em>ing these
 * masks together give the same result, and we can join such operations with
 * @em SAVE or @em WRWIM instructions.
 *
 * @date 29.04.10
 * @author Eldar Abusalimov
 */

#ifndef SPARC_WIM_H_
#define SPARC_WIM_H_

/**
 * At the boot time we are in an arbitrary register window. Therefore we reset
 * @em CWP field of @em PSR to zero and mark the @e previous window as invalid.
 * So it is possible to execute up to <em>(NWINDOWS - 1)</em> @em SAVE
 * instructions before the window overflow will occur for the first time.
 *
 * @em WIM layout:
@verbatim
 <-- RESTORE, RETT                      TRAP, SAVE -->
         +-----+---+---+---+---+---+---+---+---+
   win#: | ... | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   bits: |     |   |   |   |   |   |   | * |   |
         +-----+---+---+---+---+---+---+---+---+
                                             ^-CWP
@endverbatim
 */
#define WIM_INIT (0x1 << 1)

#endif /* SPARC_WIM_H_ */
