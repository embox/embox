/* This file is derived from newlib-1.19.0 arm port. */

#ifndef AARCH64_SETJMP_H_
#define AARCH64_SETJMP_H_

/**
 * All callee preserved registers:
 * v1 - v7, fp, ip, sp, lr, f4, f5, f6, f7
 */
#define _JBLEN 23

#ifndef __ASSEMBLER__

typedef	int __jmp_buf[_JBLEN];

#endif

#endif /*AARCH64_SETJMP_H_*/
