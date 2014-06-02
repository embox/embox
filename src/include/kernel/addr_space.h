/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#ifndef ADDR_SPACE_H_
#define ADDR_SPACE_H_

#include <hal/stack.h>

extern void addr_space_prepare_switch(void);
extern void __addr_space_finish_switch(void *);

#define addr_space_finish_switch() __addr_space_finish_switch(stack_ptr())

#endif /* ADDR_SPACE_H_ */

