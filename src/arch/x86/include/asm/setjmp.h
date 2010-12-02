/**
 * @file
 * @brief setjmp/longjmp for the x86 architecture
 *
 * @date 01.12.2010
 * @author Nikolay Korotky
 */

#ifndef X86_SETJMP_H_
#define X86_SETJMP_H_

#include <types.h>

typedef struct {
	uint32_t pc;
} __jmp_buf[1];

#endif /* X86_SETJMP_H_ */

