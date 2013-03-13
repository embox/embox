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
#include <stdint.h>

#include <embox/unit.h>

#include <mem/misc/pool.h>

#include <util/math.h>
#include <util/member.h>

#include <kernel/critical.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>
#include <kernel/panic.h>
#include <kernel/cpu.h>

#include <hal/context.h>
#include <hal/arch.h>
#include <hal/ipl.h>

#define STACK_SZ      OPTION_GET(NUMBER, thread_stack_size)
#define POOL_SZ       OPTION_GET(NUMBER, thread_pool_size)

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
	struct thread *current = thread_self();
	void *res;

	assert(!critical_allows(CRITICAL_SCHED_LOCK));

	sched_unlock_noswitch();
	ipl_enable();

	assert(!critical_inside(CRITICAL_SCHED_LOCK));

	res = current->run(current->run_arg);
	thread_exit(res);
}

int thread_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg) {
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

	thread_init(t, flags, run, arg, task_self());
	thread_context_init(t);

	if (!(flags & THREAD_FLAG_SUSPENDED)) {
		thread_launch(t);
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

static void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg, struct task *tsk) {
	assert(t);
#if 0
	assert(run);
#endif

	t->state = thread_state_init();

	t->run = run;
	t->run_arg = arg;

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

	t->task = tsk;
	list_add(&t->task_link, &tsk->threads);

	// TODO new priority range check, should fail on error. -- Eldar
	t->priority = clamp(t->priority, THREAD_PRIORITY_MIN, THREAD_PRIORITY_HIGH);
	t->initial_priority = get_sched_priority(t->task->priority, t->priority);
	t->sched_priority = t->initial_priority;

	sched_strategy_init(&t->sched);
	startq_init_thread(&t->startq_data);

	sleepq_init(&t->exit_sleepq);

	t->running_time = 0;
	t->affinity = (1 << NCPU) - 1;
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
	struct task *task = task_self();
	struct thread *thread;
	int count = 0;

	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		/* Counting number of threads in task. XXX: not the best way */
		list_for_each_entry(thread, &task->threads, task_link) {
			count++;
		}

		if ((count == 1) ||
			(count == 2 && thread_state_dead(task->main_thread->state))) {

			/* We are last thread. Unlock scheduler and exit task. */
			sched_unlock();
			task_exit(ret);
		} else {
			/* Finish scheduling of the thread */
			sched_finish(current);

			/* We can free only not main threads */
			if (current != task->main_thread) {
				if (thread_state_dead(current->state)) {
					/* Thread is detached. Should be deleted by itself. */
					thread_delete(current);
				} else {
					/* Thread is attached. Joining thread delete it.    */
					current->run_ret = ret;
					sched_wake_one(&current->exit_sleepq);
				}
			}
		}
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
	{
		if (!thread_state_exited(t->state)) {
			/* Target thread is not exited. Waiting for his exiting. */
			assert(sleepq_empty(&t->exit_sleepq));
			sched_sleep_locked_ms(&t->exit_sleepq, SCHED_TIMEOUT_INFINITE);
		}
		join_ret = t->run_ret;
		t->state = thread_state_do_detach(t->state);
		thread_delete(t);

		if (p_ret) {
			*p_ret = join_ret;
		}
	}
	sched_unlock();

	return 0;
}

int thread_detach(struct thread *t) {
	assert(t);

	sched_lock();
	{
		t->state = thread_state_do_detach(t->state);

		if (thread_state_dead(t->state)) {
			/* The target thread has finished, free it here. */
			thread_delete(t);
		}
	}
	sched_unlock();

	return 0;
}

int thread_launch(struct thread *t) {
	assert(t);

	sched_lock();
	{
		if (thread_state_started(t->state)) {
			sched_unlock();
			return -EINVAL;
		}

		if (thread_state_exited(t->state)) {
			sched_unlock();
			return -ESRCH;
		}

		sched_start(t);
	}
	sched_unlock();

	return 0;
}

int thread_terminate(struct thread *t) {
	assert(t);

	sched_lock();
	{
		if (!thread_state_exited(t->state)) {
			sched_finish(t);
		}

		if (!thread_state_detached(t->state)) {
			t->state = thread_state_do_detach(t->state);
		}

		thread_delete(t);
	}
	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_post_switch();
}

int thread_set_priority(struct thread *t, thread_priority_t new_priority) {
	assert(t);

	if ((new_priority < THREAD_PRIORITY_MIN)
			|| (new_priority > THREAD_PRIORITY_MAX)) {
		return -EINVAL;
	}

	sched_lock();
	{
		if (t->priority == new_priority) {
			sched_unlock();
			return 0;
		}
		sched_set_priority(t, get_sched_priority(t->task->priority,
					new_priority));
		t->priority = new_priority;
	}
	sched_unlock();

	return 0;
}

thread_priority_t thread_get_priority(struct thread *t) {
	assert(t);

	return t->priority;
}

/* FIXME: This operations is only for SMP */
void thread_set_affinity(struct thread *thread, unsigned int affinity) {
	thread->affinity = affinity;
}

unsigned int thread_get_affinity(struct thread *thread) {
	return thread->affinity;
}

clock_t thread_get_running_time(struct thread *thread) {
	return sched_get_running_time(thread);
}

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
		arch_idle();
	}
	return NULL;
}

struct thread *thread_init_self(void *stack, size_t stack_sz,
		thread_priority_t priority) {
	struct task *kernel_task = task_kernel_task();
	struct thread *thread = stack; /* Allocating at the bottom */

	/* Stack setting up */
	thread->stack = stack + sizeof(struct thread);
	thread->stack_sz = stack_sz - sizeof(struct thread);

	/* Global list addition and id setting up */
	thread->id = id_counter++;
	list_add_tail(&thread->thread_link, &__thread_list);

	/* General initialization and task setting up */
	thread_init(thread, 0, NULL, NULL, kernel_task);

	/* Priority setting up */
	thread->priority = priority;
	thread->sched_priority = get_sched_priority(thread->task->priority,
			thread->priority);

	return thread;
}

static int unit_init(void) {
	struct thread *idle;
	struct thread *bootstrap;
	struct task *kernel_task = task_kernel_task();
	extern char _stack_vma, _stack_len;

	id_counter = 0;

	/* TODO: priority */
	bootstrap = thread_init_self(&_stack_vma, (uint32_t) &_stack_len,
			THREAD_PRIORITY_NORMAL);

	kernel_task->main_thread = bootstrap;

	if (!(idle = thread_new())) {
		return -ENOMEM;
	}
	thread_init(idle, 0, idle_run, NULL, kernel_task);
	thread_context_init(idle);

	idle->priority = THREAD_PRIORITY_MIN;
	idle->sched_priority = get_sched_priority(idle->task->priority,
			idle->priority);

	return sched_init(bootstrap, idle);
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
	assert(t != zombie);

	if (zombie != NULL && zombie != current) {
		thread_free(zombie);
		zombie = NULL;
	}

	list_del(&t->task_link);
	list_del(&t->thread_link);

	if (t == current) {
		zombie = t;
	} else {
		thread_free(t);
	}
}

typedef union thread_pool_entry {
	struct thread thread;
	char stack[STACK_SZ];
} thread_pool_entry_t __attribute__((aligned(STACK_SZ)));

POOL_DEF(thread_pool, thread_pool_entry_t, POOL_SZ);

static struct thread *thread_alloc(void) {
	thread_pool_entry_t *block;
	struct thread *t;

	if (!(block = (thread_pool_entry_t *) pool_alloc(&thread_pool))) {
		return NULL;
	}

	t = &block->thread;

	t->stack = &block->stack + sizeof(struct thread);
	t->stack_sz = STACK_SZ - sizeof(struct thread);

	return t;
}

static void thread_free(struct thread *t) {
	thread_pool_entry_t *block;

	assert(t != NULL);

	// TODO may be this is not the best way... -- Eldar
	block = member_cast_out(t, thread_pool_entry_t, thread);
	pool_free(&thread_pool, block);
}
