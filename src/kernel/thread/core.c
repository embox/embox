/**
 * @file
 * @brief Implementation of methods in thread.h
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

#define THREAD_STACK_SIZE 0x10000

EMBOX_UNIT_INIT(threads_init);

struct thread *idle_thread;
//struct thread *thread_current();

/** Stack for idle_thread. */
static char idle_thread_stack[THREAD_STACK_SIZE];

/** Pool, containing threads. */
struct thread __thread_pool[__THREAD_POOL_SZ];

/** A mask, which shows, what places for new threads are free. */
static int thread_pool_mask[__THREAD_POOL_SZ];

/**
 * Function, which makes nothing. For idle_thread.
 */
static void idle_run(void) {
	while (true) {
		idle_thread->reschedule = true;
	}
}

static int threads_init(void) {
	idle_thread = thread_create(idle_run,
		idle_thread_stack + THREAD_STACK_SIZE);
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
static void thread_run(int par) {
	LOG_DEBUG("\nStarting Thread %d\n", thread_current()->id);
	ipl_enable();
	thread_current()->run();
	thread_stop(thread_current());
	/* NOTREACHED */
	assert(false);
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
//FIXME stack addr must be a type
struct thread *thread_create(void (*run)(void), void *stack_address) {
	uint32_t stack_addr = (uint32_t)stack_address;
	struct thread *created_thread = thread_new();
	stack_addr &= ~7;


	if (created_thread == NULL || run == NULL || stack_address == NULL) {
		return NULL;
	}
	created_thread->run = run;
	context_init(&created_thread->context, true);
	context_set_entry(&created_thread->context,
			thread_run, (int)created_thread);
	context_set_stack(&created_thread->context, (void *)stack_addr);
	created_thread->state = THREAD_STATE_STOP;
	created_thread->priority = 1;
	created_thread->need_message = false;
	#ifdef CONFIG_PP_ENABLE
	pp_add_thread( pp_cur_process, created_thread );
	#endif
	INIT_LIST_HEAD(&created_thread->sched_list);
	INIT_LIST_HEAD(&created_thread->messages);
	event_init(&created_thread->msg_event);
	LOG_DEBUG("Alloted thread id = %d\n", created_thread->id);
	return created_thread;
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

int thread_stop(struct thread *thread) {
	/* Last zombie thread. */
	static struct thread *zombie;
	LOG_DEBUG("\nI = 0x%x; D = 0x%x; Z = 0x%x;\n", (unsigned int)idle_thread,
			(unsigned int)thread, (unsigned int)zombie);
	if (thread == NULL || thread == idle_thread ||
		    thread == zombie || !thread->exist) {
		return -EINVAL;
	}
	sched_lock();
	LOG_DEBUG("\nStopping %d\n", thread->id);
	if (zombie != NULL) {
		thread_delete(zombie);
		zombie = NULL;
	}
	sched_remove(thread);
	if (thread_current() != thread) {
		thread_delete(thread);
	} else {
		zombie = thread;
		thread->state = THREAD_STATE_ZOMBIE;
	}

	LOG_DEBUG("\nZombying 0x%x\n", (unsigned int)thread);
	sched_unlock();
	return 0;
}

void thread_yield(void) {
	sched_lock();
	thread_current()->reschedule = true;
	sched_unlock();
}

