/**
 * @file
 * @brief
 *
 * @date 07.01.2011
 * @author Anton Bondarev
 */

#ifndef X86_CPU_H_
#define X86_CPU_H_

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

#endif /* X86_CPU_H_ */
