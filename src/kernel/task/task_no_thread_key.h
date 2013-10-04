/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#ifndef TASK_NO_THREAD_KEY_H_
#define TASK_NO_THREAD_KEY_H_

struct thread_key_table {
};
typedef struct thread_key_table __thread_key_table_t;

static inline void thread_key_table_create(struct task *task) {}

#endif /* TASK_NO_THREAD_KEY_H_ */
