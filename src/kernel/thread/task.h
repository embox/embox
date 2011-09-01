/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include "types.h"
//#include <drivers/vconsole.h>
#include <fs/file.h>

typedef struct task {
//	struct vconsole *own_console;

	lsof_map_t lsof_pool[CONFIG_QUANTITY_NODE];
	struct list_head *free_list;
	struct list_head *fd_cache;
} task_t;

#endif /* TASK_H_ */
