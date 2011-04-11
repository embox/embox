/**
 * @file
 * @brief Implementation of methods in api.h
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 */

#include <assert.h>
#include <errno.h>

#include <embox/unit.h>

#include <util/pool.h>
#include <kernel/critical/api.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <hal/ipl.h>

#define STACK_SZ 0x1000

EMBOX_UNIT(unit_init, unit_fini);

struct list_head __thread_list = LIST_HEAD_INIT(__thread_list);

struct thread *thread_alloc(void);
void thread_free(struct thread *t);

/**
 * Wrapper for thread start routine.
 *
 * @param thread_pointer pointer at thread.
 */
static void __attribute__((noreturn)) thread_run(int ignored) {
	struct thread *current;
	void *arg, *ret;

	assert(!critical_allows(CRITICAL_PREEMPT));

	current = thread_self();

	sched_unlock_noswitch();
	ipl_enable();

	assert(!critical_inside(CRITICAL_PREEMPT));

	arg = current->run_arg;
	ret = current->run(arg);
	current->run_ret = ret;

	thread_stop(current);

	/* NOTREACHED */assert(false);
}

int thread_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg) {
	struct thread *t;

	if (!p_thread) {
		return -EINVAL;
	}

	if (!(t = thread_alloc())) {
		return -ENOMEM;
	}

	// TODO arg. -- Eldar
	*p_thread = thread_init(t, run, t->stack, t->stack_sz);
	return 0;
}

struct thread *thread_init(struct thread *t, void *(*run)(void *),
		void *stack_address, size_t stack_size) {
	struct thread *current;

	if (!t) {
		return NULL;
	}

	if (!run || !stack_address) {
		return NULL;
	}

	context_init(&t->context, true);
	context_set_entry(&t->context, thread_run, 0 /* TODO unused arg */);
	context_set_stack(&t->context, (char *) stack_address + stack_size);

	t->run = run;

	t->susp_cnt = 0;
	t->priority = 1;

	INIT_LIST_HEAD(&t->sched_list);
	INIT_LIST_HEAD(&t->messages);
	event_init(&t->msg_event, "msg");
	event_init(&t->exit_event, "thread_exit");
	t->need_message = false;

	list_add(&t->thread_link, &__thread_list);

	// XXX WTF?
	if (NULL != (current = thread_self())) {
		t->own_console = current->own_console;
	}

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

int thread_stop(struct thread *t) {
	// XXX zombie check -- Eldar
//	static struct thread *zombie; /* Last zombie thread (if any). */

	if (!t) {
		return -EINVAL;
	}

	sched_lock();

	sched_stop(t);
	sched_wake(&t->exit_event);
	//	XXX move somewhere -- Eldar
	list_del_init(&t->thread_link);

	sched_unlock();

	return 0;
}

int thread_join(struct thread *t, void **p_ret) {
	assert(t);

	// XXX Eldar what's wrong?
	if (t->state && t->state != 0) {
		sched_sleep(&t->exit_event);
	}

	if (p_ret) {
		*p_ret = t->run_ret;
	}
	return 0; // TODO thread_join ret value
}

void thread_yield(void) {
	sched_yield();
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
	struct thread *current, *idle;
	int error;

	// TODO unused stack allocation for current thread -- Eldar
	if ((error = thread_create(&current, THREAD_FLAG_DETACHED,
			(void *(*)(void *)) -1, NULL))) {
		return error;
	}
	current->priority = THREAD_PRIORITY_MAX;

	if ((error = thread_create(&idle, THREAD_FLAG_DETACHED, idle_run, NULL))) {
		thread_free(current);
		return error;
	}
	idle->priority = THREAD_PRIORITY_MIN;

	return sched_init(current, idle);
}

static int unit_fini(void) {
	sched_lock();

	return 0;
}

#define THREAD_POOL_SZ 0x10

POOL_DEF(struct thread, thread_pool, THREAD_POOL_SZ);
POOL_DEF(char[STACK_SZ], stack_pool, THREAD_POOL_SZ);

struct thread *thread_alloc(void) {
	struct thread *t;
	void *stack;

	if (!(t = (struct thread *) static_cache_alloc(&thread_pool))) {
		return NULL;
	}

	if (!(stack = static_cache_alloc(&stack_pool))) {
		static_cache_free(&thread_pool, t);
		return NULL;
	}

	t->stack = stack;
	t->stack_sz = STACK_SZ;

	return t;
}

int thread_suspend(struct thread *t) {

	sched_lock();

	if (!t) {
		sched_unlock();
		return -EINVAL;
	}

	if (++t->susp_cnt == 1) {
		sched_suspend(t);
	}

	sched_unlock();

	return 0;
}

int thread_resume(struct thread *t) {

	sched_lock();

	if (!t) {
		sched_unlock();
		return -EINVAL;
	}

	if (!(t->susp_cnt)) {
		sched_unlock();
		return -EINVAL;
	}

	if (!(--t->susp_cnt)) {
		sched_resume(t);
	}

	sched_unlock();

	return 0;
}

void thread_free(struct thread *t) {
	assert(t != NULL);
	if (t->stack) {
		static_cache_free(&stack_pool, t->stack);
	}
	static_cache_free(&thread_pool, t);
}

