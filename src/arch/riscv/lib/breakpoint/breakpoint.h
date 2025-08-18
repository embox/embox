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

#include <riscv/exception.h>

#ifdef __riscv_c
typedef uint16_t bpt_instr_t;
#else
typedef uint32_t bpt_instr_t;
#endif

struct bpt_context {
	struct excpt_context excpt;
};

#endif /* RISCV_LIB_BREAKPOINT_H_ */
