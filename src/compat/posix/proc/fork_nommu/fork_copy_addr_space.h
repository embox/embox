/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.06.2014
 */

#ifndef FORK_COPY_ADDR_SPACE_H_
#define FORK_COPY_ADDR_SPACE_H_

#include <hal/stack.h>

extern void fork_addr_space_prepare_switch(void);
extern void fork_addr_space_finish_switch(void *safe_point);

#define __ADDR_SPACE_PREPARE_SWITCH() \
	fork_addr_space_prepare_switch()

#define __ADDR_SPACE_FINISH_SWITCH() \
	fork_addr_space_finish_switch(stack_ptr())

#endif /* FORK_COPY_ADDR_SPACE_H_ */
