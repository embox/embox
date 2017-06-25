/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.08.2013
 */

#ifndef MSP430_IPL_H_
#define MSP430_IPL_H_

typedef unsigned int __ipl_t;

static inline void ipl_init(void) {
	__asm__ __volatile__(
		"bis #0x8, r2\n\t"
		);
}

static inline unsigned int ipl_save(void) {
	unsigned int ret;
	__asm__ __volatile__(
		"mov r2, %0\n\t"
		"bic #0xfff7, %0\n\t"
		"bic #0x8, r2\n\t"
		: "=r"(ret)
		:
		:
	);

	return ret;
}

static inline void ipl_restore(unsigned int ipl) {
	__asm__ __volatile__(
		"bis %0, r2\n\t"
		:
		: "r" (ipl)
		:
	);
}

#endif /* MSP430_IPL_H_ */

