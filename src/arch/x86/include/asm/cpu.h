/**
 * @file
 * @brief
 *
 * @date 07.01.11
 * @author Anton Bondarev
 */

#ifndef X86_CPU_H_
#define X86_CPU_H_

// TODO Namespace conflict with kernel/irq, please fix it. -- Eldar
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
