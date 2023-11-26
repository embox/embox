/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */
#ifndef ARMMLIB_BREAKPOINT_H_
#define ARMMLIB_BREAKPOINT_H_

#include <stdint.h>

typedef uint16_t bpt_instr_t;

struct bpt_context {
	uint32_t r[16];
	uint32_t psr;
};

#endif /* ARMMLIB_BREAKPOINT_H_ */
