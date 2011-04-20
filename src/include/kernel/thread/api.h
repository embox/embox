/**
 * @file
 * @brief Embox threading API.
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 *          - Initial implementation
 * @author Alina Kramar
 *          - Extracting internal implementation to a separate header
 *          - Threads foreach iteration
 * @author Eldar Abusalimov
 *          - Reviewing and documenting the API
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

/** User-space thread. */
#define THREAD_FLAG_USER      (0x1 << 0)
/** Thread is created in detached state. */
#define THREAD_FLAG_DETACHED  (0x1 << 1)
/** Thread is created suspended. */
#define THREAD_FLAG_SUSPENDED (0x1 << 2)

#define THREAD_FLAG_PRIORITY_INHERIT (0x1 << 3)

#define THREAD_FLAG_PRIORITY_LOWER   (0x1 << 4)
#define THREAD_FLAG_PRIORITY_HIGHER  (0x1 << 5)

/**
 * Iterates over the list of all threads existing in the system.
 *
 * @param thread
 *   <em> struct thread * </em> iteration variable.
 */
#define thread_foreach(thread) \
	  __thread_foreach(thread)

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
 * Obtains a pointer to the calling thread.
 *
 * @return
 *   The currently executing thread.
 */
extern struct thread *thread_self(void);

/**
 * Creates a new thread.
 *
 * The new thread starts execution by invoking @a run with @a arg as its sole
 * argument.
 *
 * A memory for the thread control block and its stack is dynamically allocated
 * from an internal object pool. As far as an order of threads execution is
 * generally undefined, resource reclamation becomes a bit complicated.
 *
 * Thread becomes terminated either when it returns from its @a run function or
 * after explicit call to #thread_exit(). In order to give other threads a
 * chance to safely synchronize with the given thread or to inspect its return
 * value, the memory can't be freed immediately after the thread terminates.
 * Thus resources used by the thread may remain occupied even if it terminates.
 *
 * Kernel tracks an internal state of a thread called detach state which is
 * either @c joinable or @c detached. Resources of a terminated thread are
 * released when it becomes detached which in turn is achieved as follows:
 *
 *   - The thread was initially created in the detached state using
 *     #THREAD_FLAG_DETACHED flag.
 *
 *   - The thread has been detached using #thread_detach() method.
 *
 *   - Some thread calls #thread_join() on it.
 *
 * In other words to avoid memory leaks either #thread_join() or
 * #thread_detach() must be called for each joinable thread, so that
 * system resources for the thread can be released after it terminates.
 *
 * Once the thread becomes detached it be made joinable again. Furthermore
 * it <b>must not be used</b> at all and each reference to it should be
 * assumed invalid.
 *
 * @param p_thread
 *   Upon successful completion a pointer to the newly created thread is stored
 *   in the location referenced by this argument with the only exception of
 *   creating initially detached and running thread. In the latter case
 *   @a p_thread may be just @c NULL.
 *
 * @param flags
 *   Depending on the given @a flags the behavior may differ as follows:
 *     @n
 *   - If no flags are specified (@a 0 is passed as the value of @a flags
 *     argument), then the new thread is created in a joinable state and
 *     starts immediately.
 *     @n
 *   - The #THREAD_FLAG_SUSPENDED flag suspends the execution of the thread
 *     being created instead of starting it immediately.
 *     The effect is alike as if #thread_suspend() has been called on the
 *     thread before actually transferring the control to it. In order to start
 *     the thread, #thread_resume() should be called on it.
 *     Using this flag allows the caller to perform some additional actions
 *     on the newly created thread safely without a risk of being preempted by
 *     it. Adjusting its priority is an obvious example.
 *     @n
 *   - Presence of the #THREAD_FLAG_DETACHED flag causes the thread to be
 *     created in a detached state. If used apart #THREAD_FLAG_SUSPENDED flag
 *     then a pointer to the created thread is not stored into @a p_thread
 *     to avoid possible race conditions of using that pointer.
 * TODO memory allocation flags -- Eldar
 *
 * @param run
 *   The thread start routine. If it returns, the effect is as if there was an
 *   implicit call to #thread_exit() using the return value of @a run as the
 *   exit status.
 *
 * @param arg
 *   A value to pass to the start routine as the argument.
 *
 * @return
 *   Thread creation result.
 * @retval 0
 *   If successful.
 * @retval -EINVAL
 *   If @a p_thread pointer is @c NULL except the case discussed above.
 *   If @a run is @c NULL.
 * @retval -ENOMEM
 *   If the system lacked the necessary resources to create a new thread.
 */
extern int thread_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg);

extern int thread_detach(struct thread *thread);

/**
 * Delays execution caused thread to the end of the @a thread. Allows to save
 * the result of execution @a thread.
 *
 * @param thread
 *   Thread, execution of which will have to wait.
 *
 * @param p_ret
 *   A pointer to memory storing the result of @a thread execution.
 *
 * @return
 *   Result of joining.
 *
 * @retval 0
 *   If successful.
 */
extern int thread_join(struct thread *thread, void **p_ret);

extern void __attribute__((noreturn)) thread_exit(void *ret);

extern void thread_yield(void);

/**
 * Either suspends the @a thread. If the @a thread is already suspended,
 * increments the count of calls #thread_suspend() for this @a thread.
 *
 * @param thread
 *   Suspends the thread.
 *
 * @return
 *   Result of suspending.
 *
 * @retval 0
 *   If successful.
 */
extern int thread_suspend(struct thread *thread);

/**
 * Decrement the count of suspending. If it is zero, resumes a @a thread.
 *
 * @param thread
 *   Resumed thread.
 *
 *@return
 *   Result of resuming.
 *
 * @retval 0
 *   If successful.
 *
 * @retval -EINVAL
 *   If @thread isn't suspended.
 */
extern int thread_resume(struct thread *thread);

extern int thread_set_priority(struct thread *thread,
		thread_priority_t priority);

// XXX the following functions are considered obsolete. -- Eldar

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

extern struct thread *thread_alloc(void);

extern void thread_free(struct thread *thread);

#endif /* KERNEL_THREAD_API_H_ */
