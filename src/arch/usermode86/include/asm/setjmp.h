/**
 * @file
 * @brief setjmp/longjmp for the x86 architecture
 *
 * @date 01.12.10
 * @author Nikolay Korotky
 */

#ifndef X86_SETJMP_H_
#define X86_SETJMP_H_

#ifndef __ASSEMBLER__

#include <kernel/host.h>

#define __JMPBUF_LEN HOST_JMPBUF_LEN

typedef char __jmp_buf[__JMPBUF_LEN];

#endif /* __ASSEMBLER__ */

#endif /* X86_SETJMP_H_ */
