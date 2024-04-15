/**
 * @file
 * @brief
 *
 * @date Apr 1, 2014
 * @author Anton Bondarev
 */

#ifndef TASK_HEAP_H_
#define TASK_HEAP_H_

#include <lib/libds/dlist.h>

struct task_heap {
	 struct dlist_head mm;
};

extern struct task_heap *task_heap_get(const struct task *task);

#endif /* TASK_HEAP_H_ */
