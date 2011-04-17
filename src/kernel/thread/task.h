/**
 * @file
 *
 * @brief
 *
 * @date 17.04.2011
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include <drivers/vconsole.h>

typedef struct task {
	struct vconsole *own_console;
}task_t;

#endif /* TASK_H_ */
