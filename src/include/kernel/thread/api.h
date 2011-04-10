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
 * Thread control block.
 */
struct thread;

/**
 * Every thread can be identified using a number which is unique across the
 * system.
 */
typedef __thread_id_t thread_id_t;

typedef __thread_priority_t thread_priority_t;

/** The lowest priority is set to 255. */
#define THREAD_PRIORITY_MIN 255

/** The highest priority is zero. */
#define THREAD_PRIORITY_MAX 0

/** Total amount of valid priorities. */
#define THREAD_PRIORITY_TOTAL \
	(THREAD_PRIORITY_MIN - THREAD_PRIORITY_MAX + 1)

/**
 * Obtains a pointer to the calling thread.
 *
 * @return
 *   The currently executing thread.
 */
extern struct thread *thread_self(void);

#define thread_foreach(t) \
	  __thread_foreach(t)

/**
 * Searches for a thread by the given ID.
 *
 * @param id
 *   The thread ID to look up by.
 * @return
 *   Found thread if such exists.
 * @retval NULL
 *   If there is no thread with such ID.
 */
extern struct thread *thread_lookup(thread_id_t id);

/**
 * Performs basic thread initialization.
 *
 * @param thread
 *   The thread being initialized.
 * @param run
 *   The thread start routine.
 * @param stack_address
 *   address of thread's stack.
 * @return
 *   The first argument if the initialization has succeeded.
 * @return NULL
 *   If something went wrong (e.g. invalid arguments).
 */
extern struct thread *thread_init(struct thread *thread, void *(*run)(void *),
		void *stack_address, size_t stack_size);

/**
 * Starts a thread.
 */
extern void thread_start(struct thread *thread);

/**
 * Stops chosen thread.
 * Deletes previous zombie.
 * Makes it a zombie.
 */
extern int thread_stop(struct thread *stopped_thread);

/**
 * Changes thread's priority.
 */
extern void thread_change_priority(struct thread *thread, int new_priority);

/**
 * Switches context to another thread.
 * Currently working thread leaves CPU for some time.
 */
extern void thread_yield(void);

extern int thread_join(struct thread *thread, void **p_ret);

extern struct thread *thread_alloc(void);

extern void thread_free(struct thread *thread);

#endif /* KERNEL_THREAD_API_H_ */
