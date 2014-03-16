/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#ifndef KERNEL_TASK_THREAD_KEY_TABLE_H_
#define KERNEL_TASK_THREAD_KEY_TABLE_H_

#include <stddef.h>

struct task;

extern int task_thread_key_exist(struct task *task, size_t idx);

extern int task_thread_key_create(struct task *task, size_t *idx);

extern int task_thread_key_destroy(struct task *task, size_t idx);

#endif /* KERNEL_TASK_THREAD_KEY_TABLE_H_ */
