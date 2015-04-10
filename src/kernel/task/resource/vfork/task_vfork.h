/**
 * @file    task_vfork.h
 * @brief   Declaring task_resource_vfork
 * @date    May 16, 2014
 * @author  Anton Bondarev
 */

#ifndef TASK_VFORK_H_
#define TASK_VFORK_H_

/* struct task_vfork; must be defined in each implementation */
#include <module/embox/compat/posix/proc/vfork.h>

struct task;
extern struct task_vfork *task_resource_vfork(const struct task *task);

#endif /* TASK_VFORK_H_ */
