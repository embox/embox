#ifndef SPARC_KERNEL_REG_REG_H_
#define SPARC_KERNEL_REG_REG_H_

#ifdef CONFIG_SPARC_LEON
/*
 * Derived from Linux SPARC port.
 *
 * Copyright (C) 2004 Konrad Eisele (eiselekd@web.de), Gaisler Research
 * Copyright (C) 2004 Stefan Holst (mail@s-holst.de), Uni-Stuttgart
 */
#include <asm/asi.h>

/* do a physical address bypass write, i.e. for 0x80000000 */
static __inline__ void __leon_store_reg(unsigned long paddr,
    unsigned long value) {
	__asm__ __volatile__("sta %0, [%1] %2\n\t"
	                     :
	                     : "r"(value), "r"(paddr), "i"(ASI_LEON_BYPASS)
	                     : "memory");
}

/* do a physical address bypass load, i.e. for 0x80000000 */
static __inline__ unsigned long __leon_load_reg(unsigned long paddr) {
	unsigned long retval;
	__asm__ __volatile__("lda [%1] %2, %0\n\t"
	                     : "=r"(retval)
	                     : "r"(paddr), "i"(ASI_LEON_BYPASS));
	return retval;
}

#define __REG_STORE(addr, value) \
	__leon_store_reg((unsigned long)(addr), (unsigned long)(value))

#define __REG_LOAD(addr) __leon_load_reg((unsigned long)(addr))

#else
/* TODO not yet implemented. -- Eldar */
#endif /* CONFIG_SPARC_LEON */

#endif /* SPARC_KERNEL_REG_REG_H_ */
