/**
 * @file
 * @brief No-op stubs for interrupt/IPL interfaces.
 *
 * @date Sep 15, 2012
 * @author Eldar Abusalimov
 */

#ifndef ARCH_GENERIC_INTERRUPT_STUB_H_
#define ARCH_GENERIC_INTERRUPT_STUB_H_

#define INTERRUPT_STUB

#ifndef __ASSEMBLER__

typedef unsigned int __ipl_t;

static inline void ipl_init(void) {
}

static inline __ipl_t ipl_save(void) {
	return 0;
}

static inline void ipl_restore(__ipl_t ipl) {
}

#endif /* __ASSEMBLER__ */

#endif /* ARCH_GENERIC_INTERRUPT_STUB_H_ */
