/* This file is derived from newlib-1.19.0 arm port. */

#ifndef AARCH64_SETJMP_H_
#define AARCH64_SETJMP_H_

/**
 * All callee preserved registers:
 * SP, LR, x19-x28
 */
#define _JBLEN 12

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef	uint64_t __jmp_buf[_JBLEN];

#endif

#endif /*AARCH64_SETJMP_H_*/
