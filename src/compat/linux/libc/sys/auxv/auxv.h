/**
 * @file auxv.h
 * @brief ELF auxiliary vector access.
 *
 * Embox does not use the Linux ELF auxiliary vector. This header
 * is provided so code referencing getauxval() compiles cleanly.
 * The implementation returns 0 for all types.
 */

#ifndef COMPAT_LINUX_LIBC_SYS_AUXV_H_
#define COMPAT_LINUX_LIBC_SYS_AUXV_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern unsigned long getauxval(unsigned long type);
extern unsigned long __getauxval(unsigned long type);

__END_DECLS

#endif /* COMPAT_LINUX_LIBC_SYS_AUXV_H_ */
