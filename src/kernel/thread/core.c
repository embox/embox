/**
 * @file
 * @brief Implementation of methods in api.h
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 */

#include <assert.h>
#include <errno.h>

#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <hal/context.h>
#include <hal/arch.h>
#include <hal/ipl.h>
#include <embox/unit.h>

#ifdef CONFIG_PP_ENABLE
#include <kernel/pp.h>
#endif

#define IDLE_THREAD_STACK_SZ 0x100

EMBOX_UNIT_INIT(unit_init);

struct thread *idle_thread;

/** Pool, containing threads. */
struct thread __thread_pool[__THREAD_POOL_SZ ];

/** A mask, which shows, what places for new threads are free. */
static int thread_pool_mask[__THREAD_POOL_SZ ];

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

	idle_thread
			= thread_create(idle_run, idle_thread_stack + IDLE_THREAD_STACK_SZ);
	idle_thread->priority = THREAD_PRIORITY_MIN;
	idle_thread->state = THREAD_STATE_RUN;

	sched_add(idle_thread);

	return 0;
}

/**
 * Transforms function "run" in thread into function which can
 * execute "run" and delete thread from scheduler.
 * @param thread_pointer pointer at thread.
 */
static void thread_run(int arg) {
	ipl_enable();

	thread_current()->run();
	thread_stop(thread_current());

	/* NOTREACHED */assert(false);
}

/**
 * Allocates memory for new thread.
 *
 * @return pointer to alloted thread
 * @retval NULL if there are not free threads
 */
static struct thread *thread_new(void) {
	size_t i;
	struct thread *created_thread;
	for (i = 0; i < __THREAD_POOL_SZ; i++) {
		if (thread_pool_mask[i] == 0) {
			created_thread = __thread_pool + i;
			created_thread->id = i;
			thread_pool_mask[i] = 1;
			created_thread->exist = true;
			return created_thread;
		}
	}
	return NULL;
}
struct thread *thread_create(void(*run)(void), void *stack_address) {
	struct thread *t;

	if (!run || !stack_address) {
		return NULL;
	}

	if (!(t = thread_new())) {
		return NULL;
	}

	context_init(&t->context, true);
	context_set_entry(&t->context, thread_run, (int) t);
	context_set_stack(&t->context, stack_address);

	t->run = run;

	t->state = THREAD_STATE_STOP;
	t->priority = 1;

	INIT_LIST_HEAD(&t->sched_list);
	INIT_LIST_HEAD(&t->messages);
	event_init(&t->msg_event);
	t->need_message = false;

#ifdef CONFIG_PP_ENABLE
	pp_add_thread(pp_cur_process, created_thread);
#endif

	return t;
}

void thread_start(struct thread *thread) {
	thread->state = THREAD_STATE_RUN;
	sched_add(thread);
}

void thread_change_priority(struct thread *thread, int new_priority) {
	sched_lock();

	sched_remove(thread);
	thread->priority = new_priority;
	sched_add(thread);

	sched_unlock();
}

/**
 * Deletes chosen thread.
 */
static int thread_delete(struct thread *deleted_thread) {
	if (deleted_thread == NULL) {
		return -EINVAL;
	}
	LOG_DEBUG("\nDeleting %d\n", deleted_thread->id);
#ifdef CONFIG_PP_ENABLE
	pp_del_thread( deleted_thread->pp , deleted_thread );
#endif
	deleted_thread->state = THREAD_STATE_STOP;
	deleted_thread->exist = false;
	thread_pool_mask[deleted_thread - __thread_pool] = 0;
	return 0;
}

int thread_stop(struct thread *t) {
	static struct thread *zombie; /* Last zombie thread (if any). */

	if (!t || !t->exist) {
		return -EINVAL;
	}

	if (t == idle_thread || t == zombie) {
		return -EBUSY;
	}

	sched_lock();

	if (zombie) {
		assert(zombie != thread_current());
		thread_delete(zombie);
		zombie = NULL;
	}

	sched_remove(t);

	if (thread_current() != t) {
		thread_delete(t);
	} else {
		zombie = t;
		t->state = THREAD_STATE_ZOMBIE;
	}

	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_lock();

	thread_current()->reschedule = true;

	sched_unlock();
}

