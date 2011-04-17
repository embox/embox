/**
 * @file
 * @brief Thread management API implementation.
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 *          - Initial implementation
 * @author Alina Kramar
 *          - Thread control block memory allocation
 *          - Move state management code into the scheduler
 *          - Suspend/resume logic
 * @author Eldar Abusalimov
 *          - Reviewing and simplifying threads API
 *          - Stack allocation
 *
 * @see tests/kernel/thread/core_test.c
 */

#include <assert.h>
#include <errno.h>

#include <embox/unit.h>

#include <string.h>

#include <util/pool.h>
#include <util/structof.h>
#include <kernel/critical/api.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <hal/ipl.h>

#define STACK_SZ 0x2000

#define FLAG_EXITED (0x1 << 0)
#define FLAG_DETACHED (0x1 << 1)

EMBOX_UNIT(unit_init, unit_fini);

struct list_head __thread_list = LIST_HEAD_INIT(__thread_list);

struct thread *thread_alloc(void);
void __thread_free(struct thread *t);

static struct thread *thread_new(void);
static void thread_delete(struct thread *t);

static void __thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg);
static void __thread_ugly_init(struct thread *t);

/**
 * Wrapper for thread start routine.
 *
 * @param thread_pointer pointer at thread.
 */
static void __attribute__((noreturn)) thread_run(void) {
	struct thread *current;

	assert(!critical_allows(CRITICAL_PREEMPT));

	current = thread_self();

	sched_unlock_noswitch();
	ipl_enable();

	assert(!critical_inside(CRITICAL_PREEMPT));

	thread_exit(current->run(current->run_arg));
}

int thread_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg) {
	struct thread *t;
	int save_ptr = (flags & THREAD_FLAG_SUSPENDED) || !(flags
			& THREAD_FLAG_DETACHED);

	if (save_ptr && !p_thread) {
		return -EINVAL;
	}

	if (!run) {
		return -EINVAL;
	}

	sched_lock();

	if (!(t = thread_new())) {
		sched_unlock();
		return -ENOMEM;
	}

	__thread_init(t, flags, run, arg);

	thread_start(t);

	if (flags & THREAD_FLAG_SUSPENDED) {
		thread_suspend(t);
	}

	if (save_ptr) {
		*p_thread = t;
	}

	sched_unlock();

	return 0;
}

static void __thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg) {
	assert(t);
#if 0
	assert(t->stack);
	assert(t->stack_sz);
	assert(run);
#endif

	t->flags = 0;

	context_init(&t->context, true);
	context_set_entry(&t->context, thread_run);
	context_set_stack(&t->context, (char *) t->stack + t->stack_sz);

	t->run = run;
	t->run_arg = arg;

	t->susp_cnt = 0;

	// TODO default priority for newly created thread. -- Eldar
	t->priority = THREAD_PRIORITY_TOTAL / 2;

	INIT_LIST_HEAD(&t->sched_list);
	INIT_LIST_HEAD(&t->messages);
	event_init(&t->msg_event, "msg");
	event_init(&t->exit_event, "thread_exit");
	t->need_message = false;

	__thread_ugly_init(t);

	if (flags & THREAD_FLAG_DETACHED) {
		thread_detach(t);
	}
}

static void __thread_ugly_init(struct thread *t) {
	struct thread *current;

	// XXX WTF?
	if (NULL != (current = thread_self())) {
		memcpy(&(t->task), &(current->task), sizeof(struct task));
	}
}

struct thread *thread_init(struct thread *t, void *(*run)(void *),
		void *stack_address, size_t stack_size) {

	if (!t) {
		return NULL;
	}

	if (!run || !stack_address) {
		return NULL;
	}

	// TODO arg. -- Eldar
	__thread_init(t, 0, run, NULL);

	return t;
}

void thread_start(struct thread *t) {
	sched_start(t);
}

void thread_change_priority(struct thread *t, int priority) {
	sched_lock();

	sched_stop(t);
	t->priority = priority;
	sched_start(t);

	sched_unlock();
}

void __attribute__((noreturn)) thread_exit(void *ret) {
	struct thread *current = thread_self();

	current->run_ret = ret;

	thread_stop(current);

	/* NOTREACHED */assert(false);
}

int thread_stop(struct thread *t) {
	struct thread *joining;

	assert(t);

	sched_lock();

	sched_stop(t);

	assert(!(t->flags & FLAG_EXITED));
	t->flags |= FLAG_EXITED;
	/* Copy exit code to a joining thread (if any) so that the current thread
	 * could be safely reclaimed in case that it has already been detached
	 * without waiting for the joining thread to get the control. */
	list_for_each_entry(joining, &t->exit_event.sleep_queue, sched_list) {
		// TODO iterating over a single element or empty list. -- Eldar
		joining->join_ret = t->run_ret;
	}

	/* Wake up a joining thread (if any). */
	sched_wake(&t->exit_event);

	if (t->flags & FLAG_DETACHED) {
		thread_delete(t);
	}

	sched_unlock();

	return 0;
}

int thread_join(struct thread *t, void **p_ret) {
	struct thread *current = thread_self();
	void *join_ret;

	assert(t);

	if (t == current) {
		return -EDEADLK;
	}

	sched_lock();

	/* See notice about such assert in thread_detach(). */
	assert(!(t->flags & FLAG_DETACHED));
	t->flags |= FLAG_DETACHED;

	if (!(t->flags & FLAG_EXITED)) {
		// TODO using event internals. -- Eldar
		assert(list_empty(&t->exit_event.sleep_queue));
		sched_sleep_locked(&t->exit_event);

		/* At this point the target thread has already deleted itself.
		 * So we mustn't refer it anymore. */
		join_ret = current->join_ret;

	} else {
		/* The target thread has exited but it has not been detached yet.
		 * Get its return value and free it. */
		join_ret = t->run_ret;
		thread_delete(t);

	}

	if (p_ret) {
		*p_ret = join_ret;
	}

	sched_unlock();

	return 0;
}

int thread_detach(struct thread *t) {
	assert(t);

	sched_lock();

	/* Well, in fact this will not catch all illegal invocations,
	 * but in most cases this should be enough. */
	assert(!(t->flags & FLAG_DETACHED));
	t->flags |= FLAG_DETACHED;

	if (t->flags & FLAG_EXITED) {
		/* The target thread has finished, free it here. */
		thread_delete(t);
	}

	sched_unlock();

	return 0;
}

int thread_suspend(struct thread *t) {
	assert(t);

	sched_lock();

	if (t->flags & FLAG_EXITED) {
		sched_unlock();
		return -ESRCH;
	}

	if (++t->susp_cnt == 1) {
		sched_suspend(t);
	}

	sched_unlock();

	return 0;
}

int thread_resume(struct thread *t) {
	assert(t);

	sched_lock();

	if (t->flags & FLAG_EXITED) {
		sched_unlock();
		return -ESRCH;
	}

	if (!t->susp_cnt) {
		sched_unlock();
		return -EINVAL;
	}

	if (!(--t->susp_cnt)) {
		sched_resume(t);
	}

	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_yield();
}

int thread_set_priority(struct thread *t, thread_priority_t new) {
	if (THREAD_PRIORITY_MAX > new || new > THREAD_PRIORITY_MIN) {
		return -EINVAL;
	}

	sched_lock();

	if (t->priority == new) {
		sched_unlock();
		return 0;
	}

	sched_set_priority(t, new);

	sched_unlock();

	return 0;
}

struct thread *thread_lookup(__thread_id_t id) {
	struct thread *t;

	thread_foreach(t) {
		if (t->id == id) {
			return t;
		}
	}

	return NULL;
}

/**
 * Function, which does nothing. For idle_thread.
 */
static void *idle_run(void *arg) {
	while (true) {
		thread_yield();
	}
	return NULL;
}

static int unit_init(void) {
	static struct thread bootstrap;
	struct thread *idle;

	// TODO unused context initialization inside __thread_init. -- Eldar
	__thread_init(&bootstrap, 0, NULL, NULL);
	// TODO priority for bootstrap thread -- Eldar
	bootstrap.priority = THREAD_PRIORITY_TOTAL / 2;

	if (!(idle = thread_alloc())) {
		return -ENOMEM;
	}
	__thread_init(idle, 0, idle_run, NULL);
	idle->priority = THREAD_PRIORITY_MIN;

	return sched_init(&bootstrap, idle);
}

static int unit_fini(void) {
	sched_lock();

	return 0;
}

static struct thread *thread_new(void) {
	struct thread *t;

	if (!(t = thread_alloc())) {
		return NULL;
	}

#if 0
	list_add(&t->thread_link, &__thread_list);
#endif
	return t;
}

static void thread_delete(struct thread *t) {
	static struct thread *zombie;
	struct thread *current = thread_self();

	assert(t);

	if (zombie != NULL) {
		assert(zombie != current);
		__thread_free(zombie);
		zombie = NULL;
	}

#if 0
	list_del_init(&t->thread_link);
#endif

	if (t == current) {
		zombie = t;
	} else {
		__thread_free(t);
	}
}

#define THREAD_POOL_SZ 0x10

union thread_pool_entry {
	struct thread thread;
	char stack[STACK_SZ];
};

POOL_DEF(union thread_pool_entry, thread_pool, THREAD_POOL_SZ);

struct thread *thread_alloc(void) {
	union thread_pool_entry *block;
	struct thread *t;
	void *stack;

	if (!(block = (union thread_pool_entry *) static_cache_alloc(&thread_pool))) {
		return NULL;
	}

	t = &block->thread;

	t->stack = &block->stack;
	t->stack_sz = STACK_SZ;

	return t;
}

void __thread_free(struct thread *t) {
	union thread_pool_entry *block;

	assert(t != NULL);

	// TODO may be this is not the best way... -- Eldar
	block = structof(t, union thread_pool_entry, thread);
	static_cache_free(&thread_pool, block);
}

void thread_free(struct thread *t) {
	thread_delete(t);
}

