/**
 * @file
 * @brief ISO C99 Standard: 7.13 Nonlocal jumps.
 * @details Provides "non-local jumps" facility.
 *
 * @date 01.02.10
 * @author Eldar Abusalimov
 */

#ifndef COMPAT_LIBC_SETJMP_H_
#define COMPAT_LIBC_SETJMP_H_

#include <asm/setjmp.h>

#include <sys/cdefs.h>
#include <compiler.h>

__BEGIN_DECLS

/**
 * An array type suitable for holding the information needed to restore a
 * calling environment.
 */
typedef __jmp_buf jmp_buf;

/**
 * Sets up the local buffer and initializes it for the jump. This routine saves
 * the program's calling environment in the environment buffer specified by the
 * @c env argument for later use by #longjmp().
 *
 * @param env #jmp_buf to save environment information to
 *
 * @return depends on invocation type
 * @retval 0 if the return is from a direct invocation
 * @retval non-zero if the return is from a call to @c longjmp. In this case
 *         the return value is the same as passed to @c longjmp as it's second
 *         argument.
 */
extern int setjmp(jmp_buf env);

/**
 * Restores the context of the environment buffer @c env that was saved by
 * invocation of the #setjmp() routine. The value specified by
 * @c value is passed from @c longjmp to @c setjmp. After @c longjmp is
 * completed, program execution continues as if the corresponding invocation of
 * @c setjmp had just returned. If the @c value passed to @c longjmp is 0,
 * @c setjmp will behave as if it had returned 1; otherwise, it will behave as
 * if it had returned @c value.
 *
 * @param env #jmp_buf to restore the context from
 * @param value the return value for @c setjmp call
 */
extern void _NORETURN longjmp(jmp_buf env, int value);

__END_DECLS

#endif /* COMPAT_LIBC_SETJMP_H_ */
