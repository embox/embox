/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#ifndef KERNEL_ADDR_SPACE_H_
#define KERNEL_ADDR_SPACE_H_

#include <module/embox/kernel/addr_space.h>

#define ADDR_SPACE_PREPARE_SWITCH() __ADDR_SPACE_PREPARE_SWITCH()

#define ADDR_SPACE_FINISH_SWITCH() __ADDR_SPACE_FINISH_SWITCH()

#define ADDR_SPACE_PREPARE_APP(mod) __ADDR_SPACE_PREPARE_APP(mod)

#define ADDR_SPACE_FINISH_APP() __ADDR_SPACE_FINISH_APP()

#endif /* KERNEL_ADDR_SPACE_H_ */
