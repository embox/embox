/**
 * @file
 *
 * @date  14.05.2019
 * @author Dmitry Kurbatov
 */

#ifndef RISCV_KERNEL_IPL_H_
#define RISCV_KERNEL_IPL_H_

#include <stdint.h>

#include <asm/csr.h>

typedef unsigned long __ipl_t;

static inline void ipl_init(void) {
	csr_set(CSR_STATUS, CSR_STATUS_IE);
}

static inline __ipl_t ipl_save(void) {
	return csr_read_clear(CSR_STATUS, CSR_STATUS_IE);
}

static inline void ipl_restore(__ipl_t ipl) {
	csr_write(CSR_STATUS, ipl);
}

#endif /* RISCV_KERNEL_IPL_H_ */
