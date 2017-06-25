/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    28.05.2014
 */

#ifndef TASK_RESOURCE_MODULE_PTR_H_
#define TASK_RESOURCE_MODULE_PTR_H_

struct mod;
struct task;

extern void task_self_module_ptr_set(const struct mod *);

extern const struct mod *task_module_ptr_get(struct task *);

#endif /* TASK_RESOURCE_MODULE_PTR_H_ */

