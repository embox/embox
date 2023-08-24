/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */
#ifndef RISCV_LIB_BREAKPOINT_H_
#define RISCV_LIB_BREAKPOINT_H_

#include <stdint.h>

#ifdef __riscv_c
typedef uint16_t bpt_instr_t;
#else
typedef uint32_t bpt_instr_t;
#endif

struct bpt_context {
	unsigned long r[31];
	unsigned long mstatus;
	unsigned long pc;
};

#endif /* RISCV_LIB_BREAKPOINT_H_ */
