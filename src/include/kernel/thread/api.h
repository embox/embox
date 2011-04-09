/**
 * @file
 * @brief Defines structure of threads and methods,
 * which allows working with them.
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 *         - Initial implementation
 * @author Alina Kramar
 *         - Extracting internal implementation to separate header
 *         - Thread iteration code
 */

#ifndef KERNEL_THREAD_API_H_
#define KERNEL_THREAD_API_H_

#include __impl_x(kernel/thread/api_impl.h)

/**
 * Structure, describing threads.
 */
struct thread;

typedef __thread_id_t thread_id_t;
typedef __thread_priority_t thread_priority_t;

/** The highest priority is zero. */
#define THREAD_PRIORITY_MAX 0x0

/** The lowest priority is set to 255. */
#define THREAD_PRIORITY_MIN 0xFF

/** Total amount of valid priorities. */
#define THREAD_PRIORITY_TOTAL \
	(THREAD_PRIORITY_MIN - THREAD_PRIORITY_MAX + 1)

/**
 * Thread, which does nothing.
 * Is used to be working when there is no another process.
 */
extern struct thread *idle_thread;

/**
 * Thread, which is working now.
 */
extern struct thread *thread_current(void);

#define thread_foreach(t) \
	  __thread_foreach(t)

extern struct thread *thread_get_by_id(thread_id_t id);

/**
 * Creates new thread
 *
 * @param run function executed in created thread.
 * @param stack_address address of thread's stack.
 * @return pointer to new thread if all parameters are correct.
 * @return NULL if one of parameters is NULL or all places for threads are occupied.
 */
extern struct thread *thread_init(struct thread *thread, void(*run)(void), void *stack_address);

/**
 * Starts a thread.
 */
extern void thread_start(struct thread *thread);

/**
 * Changes thread's priority.
 */
extern void thread_change_priority(struct thread *thread, int new_priority);

/**
 * Stops chosen thread.
 * Deletes previous zombie.
 * Makes it a zombie.
 */
extern int thread_stop(struct thread *stopped_thread);

/**
 * Switches context to another thread.
 * Currently working thread leaves CPU for some time.
 */
extern void thread_yield(void);

extern void thread_join(struct thread *thread);

extern struct thread *thread_alloc(void);

extern void thread_free(struct thread *thread);

#endif /* KERNEL_THREAD_API_H_ */
