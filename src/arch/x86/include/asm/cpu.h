/**
 * @file
 *
 * @brief
 *
 * @date 07.01.2011
 * @author Anton Bondarev
 */

#ifndef CPU_H_
#define CPU_H_


static inline void irq_enable(void) {
	__asm__ __volatile__(
		"sti ":	:
	);
}

static inline void irq_disable(void) {
	__asm__ __volatile__(
		"cti ":	:
	);
}


#endif /* CPU_H_ */
