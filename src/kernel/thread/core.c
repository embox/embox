/**
 * @file
 * @brief Thread management API implementation.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 *          - Initial implementation
 * @author Alina Kramar
 *          - Thread control block memory allocation
 *          - Move state management code into the scheduler
 *          - Suspend/resume logic
 * @author Eldar Abusalimov
 *          - Reviewing and simplifying threads API
 *          - Stack allocation
 * @author Anton Kozlov
 *          - Tasks binding
 *
 * @see tests/kernel/thread/core_test.c
 */

#include <assert.h>
#include <errno.h>

#include <embox/unit.h>

#include <string.h>

#include <mem/misc/pool.h>
#include <util/math.h>
#include <util/member.h>
#include <kernel/critical.h>
#include <kernel/thread/api.h>
#include <kernel/task.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>
#include <kernel/panic.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <hal/ipl.h>

#ifdef CONFIG_THREAD_STACK_SIZE
# define STACK_SZ CONFIG_THREAD_STACK_SIZE
#else
# define STACK_SZ 0x2000
#endif

EMBOX_UNIT(unit_init, unit_fini);

struct list_head __thread_list = LIST_HEAD_INIT(__thread_list);

static int id_counter;

static void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg, struct task *tsk);
static void thread_context_init(struct thread *t);

static struct thread *thread_new(void);
static void thread_delete(struct thread *t);

static struct thread *thread_alloc(void);
static void thread_free(struct thread *t);

/**
 * Wrapper for thread start routine.
 */
static void __attribute__((noreturn)) thread_trampoline(void) {
	struct thread *current;

	assert(!critical_allows(CRITICAL_SCHED_LOCK));

	current = thread_self();

	sched_unlock_noswitch();
	ipl_enable();

	assert(!critical_inside(CRITICAL_SCHED_LOCK));

	thread_exit(current->run(current->run_arg));
}

static int thread_create_task(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg, struct task *tsk) {
	struct thread *t;
	int save_ptr = (flags & THREAD_FLAG_SUSPENDED)
			|| !(flags & THREAD_FLAG_DETACHED);

	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (flags & THREAD_FLAG_PRIORITY_HIGHER)) {
		return -EINVAL;
	}

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

	thread_init(t, flags, run, arg, tsk);
	thread_context_init(t);

	if (!(flags & THREAD_FLAG_SUSPENDED)) {
		thread_resume(t);
	}

	if (flags & THREAD_FLAG_DETACHED) {
		thread_detach(t);
	}

	if (save_ptr) {
		*p_thread = t;
	}

	sched_unlock();

	return 0;
}

int thread_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg) {
	struct task *tsk = task_self();

	return thread_create_task(p_thread, flags, run, arg, tsk);
}

static void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg, struct task *tsk) {
	assert(t);
#if 0
	assert(run);
#endif

	t->state = thread_state_init();

	t->run = run;
	t->run_arg = arg;

	t->suspend_count = 1;

	if (flags & THREAD_FLAG_PRIORITY_INHERIT) {
		t->priority = thread_self()->priority;
	} else {
		t->priority = THREAD_PRIORITY_DEFAULT;
	}

	if (flags & THREAD_FLAG_PRIORITY_LOWER) {
		t->priority--;
	} else if (flags & THREAD_FLAG_PRIORITY_HIGHER) {
		t->priority++;
	}

	if (flags & THREAD_FLAG_IN_NEW_TASK) {
		task_create(&tsk, tsk);
	}

	t->task = tsk;
	list_add(&t->task_link, &tsk->threads);

	// TODO new priority range check, should fail on error. -- Eldar
	t->initial_priority = clamp(t->priority,
			THREAD_PRIORITY_MIN, THREAD_PRIORITY_HIGH);
	t->priority = t->initial_priority;

	sched_strategy_init(&t->sched);
	slist_link_init(&t->startq_link);

	INIT_LIST_HEAD(&t->messages);
	event_init(&t->msg_event, "msg");
	event_init(&t->exit_event, "thread_exit");
	t->need_message = false;

	t->running_time = 0;
}

static void thread_context_init(struct thread *t) {
	assert(t);
	assert(t->stack);
	assert(t->stack_sz);

	context_init(&t->context, true);
	context_set_entry(&t->context, thread_trampoline);
	context_set_stack(&t->context, (char *) t->stack + t->stack_sz);
}

void __attribute__((noreturn)) thread_exit(void *ret) {
	struct thread *current = thread_self();
	struct thread *joining;
	struct sleepq *exit_sq;

	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();

	sched_suspend(current);

	current->state = thread_state_do_exit(current->state);
	/* Copy exit code to a joining thread (if any) so that the current thread
	 * could be safely reclaimed in case that it has already been detached
	 * without waiting for the joining thread to get the control. */
	// XXX just for now. -- Eldar
	exit_sq = &current->exit_event.sleepq;
	if (!sleepq_empty(exit_sq)) {
		joining = sleepq_get_thread(exit_sq);
		joining->join_ret = ret;
	}

	/* Wake up a joining thread (if any). */
	sched_wake(&current->exit_event);

	if (thread_state_dead(current->state)) {
		thread_delete(current);
	} else {
		current->run_ret = ret;
	}

	sched_unlock();

	/* NOTREACHED */
	panic("Returning from thread_exit()");
}

int thread_join(struct thread *t, void **p_ret) {
	struct thread *current = thread_self();
	void *join_ret;

	assert(t);

	if (t == current) {
		return -EDEADLK;
	}

	sched_lock();

	t->state = thread_state_do_detach(t->state);

	if (thread_state_dead(t->state)) {
		/* The target thread has exited but it has not been detached before
		 * thread_join() call. Get its return value and free it. */
		join_ret = t->run_ret;
		thread_delete(t);

	} else {
		assert(sleepq_empty(&t->exit_event.sleepq));
		sched_sleep_locked(&t->exit_event);

		/* At this point the target thread has already deleted itself.
		 * So we mustn't refer it anymore. */
		join_ret = current->join_ret;

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

	t->state = thread_state_do_detach(t->state);

	if (thread_state_dead(t->state)) {
		/* The target thread has finished, free it here. */
		thread_delete(t);
	}

	sched_unlock();

	return 0;
}

int thread_suspend(struct thread *t) {
	assert(t);

	sched_lock();

	if (thread_state_exited(t->state)) {
		sched_unlock();
		return -ESRCH;
	}

	if (!(t->suspend_count++)) {
		sched_suspend(t);
	}

	sched_unlock();

	return 0;
}

int thread_resume(struct thread *t) {
	assert(t);

	sched_lock();

	if (thread_state_exited(t->state)) {
		sched_unlock();
		return -ESRCH;
	}

	if (!t->suspend_count) {
		sched_unlock();
		return -EINVAL;
	}

	if (!(--t->suspend_count)) {
		sched_resume(t);
	}

	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_yield();
}

int thread_set_priority(struct thread *t, thread_priority_t new) {
	if (new < THREAD_PRIORITY_MIN || THREAD_PRIORITY_MAX < new) {
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

void* thread_get_stack_start(struct thread const* t) { return t -> stack; }

struct thread *thread_lookup(thread_id_t id) {
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
	struct task *default_task;
	id_counter = 0;

	bootstrap.id = id_counter++;
	list_add_tail(&bootstrap.thread_link, &__thread_list);

	default_task = task_default_get();

	thread_init(&bootstrap, 0, NULL, NULL, default_task);
	// TODO priority for bootstrap thread -- Eldar
	bootstrap.priority = THREAD_PRIORITY_NORMAL;

	if (!(idle = thread_new())) {
		return -ENOMEM;
	}
	thread_init(idle, 0, idle_run, NULL, default_task);
	thread_context_init(idle);

	bootstrap.task = default_task;
	idle->task = default_task;

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

	t->id = id_counter++;
	list_add_tail(&t->thread_link, &__thread_list);

	return t;
}

static void thread_delete(struct thread *t) {
	static struct thread *zombie;
	struct thread *current = thread_self();

	assert(t);
	assert(thread_state_dead(t->state));

	if (zombie != NULL) {
		assert(zombie != current);
		thread_free(zombie);
		zombie = NULL;
	}

	list_del_init(&t->thread_link);

	if (t == current) {
		zombie = t;
	} else {
		thread_free(t);
	}
}

#define THREAD_POOL_SZ 0x10

union thread_pool_entry {
	struct thread thread;
	char stack[STACK_SZ];
};

POOL_DEF(thread_pool, union thread_pool_entry, THREAD_POOL_SZ);

static struct thread *thread_alloc(void) {
	union thread_pool_entry *block;
	struct thread *t;

	if (!(block = (union thread_pool_entry *) pool_alloc(&thread_pool))) {
		return NULL;
	}

	t = &block->thread;

	t->stack = &block->stack;
	t->stack_sz = STACK_SZ;

	return t;
}

static void thread_free(struct thread *t) {
	union thread_pool_entry *block;

	assert(t != NULL);

	// TODO may be this is not the best way... -- Eldar
	block = member_cast_out(t, union thread_pool_entry, thread);
	pool_free(&thread_pool, block);
}
