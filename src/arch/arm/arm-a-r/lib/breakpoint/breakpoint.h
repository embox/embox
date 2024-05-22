/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */
#ifndef ARMLIB_BREAKPOINT_H_
#define ARMLIB_BREAKPOINT_H_

#include <stdint.h>

typedef uint32_t bpt_instr_t;

struct bpt_context {
	uint32_t r[16];
	uint32_t psr;
};

#endif /* ARMLIB_BREAKPOINT_H_ */
