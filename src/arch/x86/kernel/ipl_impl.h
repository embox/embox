/**
 * @file
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#ifndef X86_IPL_IMPL_H_
#define X86_IPL_IMPL_H_

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <asm/flags.h>

#include <drivers/irqctrl.h>

typedef uint32_t __ipl_t;

// XXX Now it acts as a flag indicating whether module is initialized. -- Eldar
extern __ipl_t __cur_ipl;

extern void apic_init(void);

static inline void ipl_init(void) {
	if (!__cur_ipl) {
		apic_init();
	}
	__cur_ipl = 1;

	__asm__ __volatile__ ("sti;\n\t" : : : "memory");
}

static inline unsigned int ipl_save(void) {
	unsigned int ret;

	__asm__ __volatile__ (
		"pushf;\n\t"
		"pop %0;\n\t"
		"cli;\n\t"
		: "=&r" (ret)
		:
		: "memory"
	);

	return ret;
}

static inline void ipl_restore(unsigned int ipl) {
	if (ipl & X86_EFLAGS_IF) {
		__asm__ __volatile__ ("sti;\n\t" : : : "memory");
	} else {
		__asm__ __volatile__ ("cli;\n\t" : : : "memory");
	}
}

#endif /* __ASSEMBLER__ */

#endif /* X86_IPL_IMPL_H_ */
