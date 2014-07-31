/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

#ifndef HAL_VFORK_H_
#define HAL_VFORK_H_

struct task;
extern int vfork_child_start(struct task *child);
extern void vfork_child_done(struct task *task, void * (*run)(void *), void *arg);
extern void *task_exit_callback(void *arg);
extern void *task_exec_callback(void *arg);

#endif /* HAL_VFORK_H_ */

