/**
 * @file
 * @brief cpsr utilities
 *
 * @date 21.07.2010
 * @author Anton Kozlov
 */

static inline unsigned __get_cpsr(void) {
	unsigned long retval;
	asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */);
	return retval;
}

static inline void __set_cpsr(unsigned val) {
	asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val) );
}

