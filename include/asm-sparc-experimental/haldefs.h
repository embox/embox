/**
 * @file haldefs.h
 *
 * @brief Provides necessary definitions needed by HAL.
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_HALDEFS_H_
#define SPARC_HALDEFS_H_

#define __CLOCK_NRS_TOTAL 2
typedef unsigned char __clock_nr_t;

#define __INTERRUPT_NRS_TOTAL 16
typedef unsigned char __interrupt_nr_t;

typedef unsigned int __irqlevel_t;

#ifdef CONFIG_SPARC_LEON

#include "leon/regs.h"

#define __REG_STORE(addr, value) \
		LEON_REGSTORE_PA(addr, value)

#define __REG_LOAD(addr) \
		LEON_REGLOAD_PA(addr)

#define __REG_ORIN(addr, mask) \
		LEON_REGSTORE_OR_PA(addr, mask)

#define __REG_ANDIN(addr, mask) \
		LEON_REGSTORE_AND_PA(addr, mask)

#endif /* CONFIG_SPARC_LEON */

#endif /* SPARC_HALDEFS_H_ */
