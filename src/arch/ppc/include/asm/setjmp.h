/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_SETJMP_H_
#define MIPS_SETJMP_H_

#ifndef __ASSEMBLER__
typedef struct {
	unsigned int r1;
} __jmp_buf[1];

#else




#endif

#endif /* MIPS_SETJMP_H_ */
