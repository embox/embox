/**
 * @file
 *
 * @date 03.07.2010
 * @author Anton Anton
 */

#ifndef STACK_H_
#define STACK_H_

#ifndef __ASSEMBLER__

#include "types.h"

typedef struct stack_frame {
	uint32_t regs[32];
	uint32_t rmsr;
} stack_frame_t;

#endif /*__ASSEMBLER__*/

#endif /* STACK_H_ */
