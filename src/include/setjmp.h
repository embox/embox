/**
 * @file
 * @brief Provides "non-local jumps" facility.
 *
 * @date 01.02.2010
 * @author Eldar Abusalimov
 */

#ifndef SETJMP_H_
#define SETJMP_H_

#include <asm/setjmp.h>

/**
 * An array type suitable for holding the information needed to restore a
 * calling environment.
 */
typedef __jmp_buf jmp_buf;

/**
 * Sets up the local buffer and initializes it for the jump. This routine saves
 * the program's calling environment in the environment buffer specified by the
 * @c env argument for later use by @link longjmp() @endlink .
 *
 * @param env @link jmp_buf @endlink to save environment information to
 *
 * @return depends on invocation type
 * @retval 0 if the return is from a direct invocation
 * @retval non-zero if the return is from a call to @c longjmp. In this case
 *         the return value is the same as passed to @c longjmp as it's second
 *         argument.
 */
int setjmp(jmp_buf env);

/**
 * Restores the context of the environment buffer @c env that was saved by
 * invocation of the @link setjmp() @endlink routine. The value specified by
 * @c value is passed from @c longjmp to @c setjmp. After @c longjmp is
 * completed, program execution continues as if the corresponding invocation of
 * @c setjmp had just returned. If the @c value passed to @c longjmp is 0,
 * @c setjmp will behave as if it had returned 1; otherwise, it will behave as
 * if it had returned @c value.
 *
 * @param env @link jmp_buf @endlink to restore the context from
 * @param value the return value for @c setjmp call
 */
void longjmp(jmp_buf env, int value) __attribute__ ((noreturn));

#endif /* SETJMP_H_ */
