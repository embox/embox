/**
 * @file
 *
 * @date 5.02.2026
 * @author: Anton Bondarev
 */

#ifndef TASK_RESOURCE_TIMER_TABLE_H_
#define TASK_RESOURCE_TIMER_TABLE_H_

#include <sys/types.h>
#include <signal.h>
#include <time.h>

#include <sys/cdefs.h>

struct sys_timer;
struct task;

struct task_resource_timer_desc {
	timer_t timerid;
	struct sigevent sigevent;
	struct itimerspec value;
	struct itimerspec ovalue;
	struct sys_timer *sys_timer;
	struct task *task;
	int overrun_count;
	struct timespec next_value;
};

struct task;

__BEGIN_DECLS

extern int task_resource_timer_table_alloc_index(const struct task *task);

extern int task_resource_timer_table_free_index(const struct task *task, int idx);

extern int task_resource_timer_table_add(const struct task *task, int idx,
										struct task_resource_timer_desc *desc);
extern int task_resource_timer_table_del(const struct task *task, int idx);

extern struct task_resource_timer_desc *task_resource_timer_table_get(
											const struct task *task, int idx);

extern struct timer_table *task_resource_timer_table(const struct task *task);

__END_DECLS

#endif /* TASK_RESOURCE_TIMER_TABLE_H_ */
