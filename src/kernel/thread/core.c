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

#ifdef CONFIG_PP_ENABLE
#include <kernel/pp.h>
#endif

#define IDLE_THREAD_STACK_SZ 0x100

EMBOX_UNIT_INIT(unit_init);

struct thread *idle_thread;

struct list_head __thread_list = LIST_HEAD_INIT(__thread_list);

static struct thread *thread_alloc(void);
static void thread_free(struct thread *t);

/**
 * Function, which does nothing. For idle_thread.
 */
static void idle_run(void) {
	while (true) {
		thread_yield();
	}
}

static int unit_init(void) {
	static char idle_thread_stack[IDLE_THREAD_STACK_SZ];
	struct thread *current;

	if (!(current = thread_create((void(*)(void)) -1, (void *) -1))) {
		return -ENOMEM;
	}
	// FIXME priority
	current->priority = THREAD_PRIORITY_MIN;

	if (!(idle_thread = thread_create(idle_run,
			idle_thread_stack + IDLE_THREAD_STACK_SZ))) {
		thread_free(current);
		return -ENOMEM;
	}
	idle_thread->priority = THREAD_PRIORITY_MIN;

	sched_init(current, idle_thread);

	return 0;
}

/**
 * Transforms function "run" in thread into function which can
 * execute "run" and delete thread from scheduler.
 * @param thread_pointer pointer at thread.
 */
static void thread_run(int arg) {
	struct thread *current = (struct thread *) arg;

	assert(current == thread_current());

	sched_unlock_noswitch();
	ipl_enable();

	assert(!critical_inside(CRITICAL_PREEMPT));

	current->run();
	thread_stop(current);

	/* NOTREACHED */assert(false);
}

struct thread *thread_create(void(*run)(void), void *stack_address) {
	struct thread *t;

	if (!run || !stack_address) {
		return NULL;
	}

	if (!(t = thread_alloc())) {
		return NULL;
	}

	context_init(&t->context, true);
	context_set_entry(&t->context, thread_run, (int) t);
	context_set_stack(&t->context, stack_address);

	t->run = run;

	t->state = THREAD_STATE_TERMINATE;
	t->priority = 1;

	INIT_LIST_HEAD(&t->sched_list);
	INIT_LIST_HEAD(&t->messages);
	event_init(&t->msg_event);
	t->need_message = false;

#ifdef CONFIG_PP_ENABLE
	pp_add_thread(pp_cur_process, t);
#endif

	return t;
}

void thread_start(struct thread *thread) {
	sched_add(thread);
}

void thread_change_priority(struct thread *thread, int new_priority) {
	sched_lock();

	sched_remove(thread);
	thread->priority = new_priority;
	sched_add(thread);

	sched_unlock();
}

int thread_stop(struct thread *t) {
	static struct thread *zombie; /* Last zombie thread (if any). */

	if (!t) {
		return -EINVAL;
	}

	if (t == idle_thread || t == zombie) {
		return -EBUSY;
	}

	sched_lock();

	if (zombie) {
		assert(zombie != thread_current());
		thread_free(zombie);
		zombie = NULL;
	}

	sched_remove(t);

	if (thread_current() != t) {
		thread_free(t);
	} else {
		zombie = t;
	}

	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_lock();

	thread_current()->reschedule = true;

	sched_unlock();
}

POOL_DEF(struct thread, thread_pool, __THREAD_POOL_SZ);

static struct thread *thread_alloc(void) {
	struct thread *t;

	if (!(t = (struct thread *) static_cache_alloc(&thread_pool))) {
		return NULL;
	}

	list_add(&t->thread_link, &__thread_list);

	return t;
}

static void thread_free(struct thread *t) {
	assert(t != NULL);
	list_del_init(&t->thread_link);
	static_cache_free(&thread_pool, t);
}

