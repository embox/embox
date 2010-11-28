/**
 * @file
 *
 * @date 03.07.2010
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_STACK_H_
#define MICROBLAZE_STACK_H_

#ifndef __ASSEMBLER__

#include <types.h>

typedef struct stack_frame {
	uint32_t regs[32];
	uint32_t rmsr;
} stack_frame_t;

#endif /*__ASSEMBLER__*/

#endif /* MICROBLAZE_STACK_H_ */
