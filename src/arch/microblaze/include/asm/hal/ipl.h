#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <asm/msr.h>
#include <hal/interrupt.h>

typedef unsigned int __ipl_t;

inline static void ipl_init(void) {
	irqc_set_mask(~(__interrupt_mask_t)0);
}

inline static unsigned int ipl_save(void) {
	return irqc_get_mask();
}

inline static void ipl_restore(unsigned int ipl) {
	irqc_set_mask(ipl);
}
