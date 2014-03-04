/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_


#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/task/task_priority.h>
#include <module/embox/kernel/task/api.h>


struct thread;

struct task;

__BEGIN_DECLS

extern int task_get_id(const struct task *tsk);

extern const char * task_get_name(const struct task *tsk);

extern struct thread * task_get_main(const struct task *tsk);

extern task_priority_t task_get_priority(const struct task *tsk);

extern int task_set_priority(struct task *tsk, task_priority_t new_priority);

extern clock_t task_get_clock(const struct task *tsk);

extern void task_set_clock(struct task *tsk, clock_t new_clock);

/**
 * @brief Get self task (task which current execution thread associated with)
 *
 * @return Pointer to self task
 */
extern struct task *task_self(void);

/**
 * @brief Create new task
 *
 * @param name Task name
 * @param run Task main function
 * @param arg run argument
 * @return pid of the new task
 */
extern struct task *task_self(void);

extern int new_task(const char *name, void *(*run)(void *), void *arg);

extern int new_task(const char *name, void *(*run)(void *), void *arg);

extern void task_init(struct task *tsk, int id, const char *name,
		struct thread *main_thread, task_priority_t priority);

/**
 * @brief Exit from current task
 *
 * @param res Return code
 */
extern void __attribute__((noreturn)) task_exit(void *res);

extern int task_notify_switch(struct thread *prev, struct thread *next);

extern int task_waitpid(pid_t pid);

__END_DECLS

#include <util/dlist.h>

#define task_foreach_thread(th, tsk) \
	th = task_get_main(tsk); \
	for (struct thread *nxt = dlist_entry(th->thread_link.next, \
				struct thread, thread_link), \
			*loop = NULL; \
			(th != task_get_main(tsk)) || !loop; \
			loop = th, th = nxt, nxt = dlist_entry(th->thread_link.next, \
				struct thread, thread_link))

#include <kernel/task/task_table.h>

#define task_foreach(tsk) \
	tsk = task_table_get(0); \
	for (int tid = 0; tsk != NULL; \
			++tid, tsk = task_table_get(task_table_get_first(tid)))



#endif /* TASK_H_ */
