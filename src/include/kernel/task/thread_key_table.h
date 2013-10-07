/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_KEY_TABLE_H_
#define THREAD_KEY_TABLE_H_

struct task;

#include <module/embox/kernel/task/task_key_table.h>

typedef __thread_key_table_t thread_key_table_t;

extern void task_thread_key_init(struct task *task);

extern int task_thread_key_exist(struct task *task, size_t idx);

extern int task_thread_key_create(struct task *task, size_t *idx);

extern int task_thread_key_destroy(struct task *task, size_t idx);

#endif /* THREAD_KEY_TABLE_H_ */
