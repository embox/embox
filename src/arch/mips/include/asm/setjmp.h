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

typedef struct {
	unsigned int s[9];     /* common registers */
	unsigned int fp;       /* frame pointer */
	unsigned int ret_addr; /* return address */
	unsigned int gp;       /* global pointer */
	unsigned int sp;       /* stack pointer */
} __jmp_buf[1];


#endif /* MIPS_SETJMP_H_ */
