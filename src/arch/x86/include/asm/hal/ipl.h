/**
 * @file
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <types.h>

#include <hal/interrupt.h>

typedef uint32_t __ipl_t;

extern __ipl_t __cur_ipl;

extern void apic_init(void);

static inline void ipl_init(void) {
	__cur_ipl = 0;
	apic_init();
	__asm__ __volatile__ ("sti;\n\t");
}

static inline unsigned int ipl_save(void) {
	unsigned int ret;
	__asm__ __volatile__ ("cli;\n\t");
	ret = __cur_ipl;
	__cur_ipl = 15;

	return ret;
}

static inline void ipl_restore(unsigned int ipl) {
	__asm__ __volatile__("cli;\n\t");
	__cur_ipl = ipl;
	if (0 == ipl) {
		__asm__ __volatile__("sti;\n\t");
	}
}
