/**
 * @file
 * @brief
 *
 * @date 19.11.12
 * @author Anton Bulychev
 */

#include <errno.h>
#include <assert.h>

#include <mem/misc/pool.h>
#include <kernel/usermode.h>
#include <kernel/thread.h>
#include <util/err.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/task.h>

/* Simultaneous number of task creation */
#define UE_DATA_POOL_SIZE 5

POOL_DEF(ue_data_pool, struct ue_data, UE_DATA_POOL_SIZE);

static void usermode_trampoline(struct ue_data *data) {
	struct ue_data s_data;

	sched_lock();
	{
		/* Saving data on the stack and free from pool */
		s_data = (struct ue_data) {
			.ip = data->ip,
			.sp = data->sp,
		};
		pool_free(&ue_data_pool, data);
	}
	sched_unlock();

	usermode_entry(&s_data);
}

#define TRAMPOLINE ((void * (*)(void *)) usermode_trampoline)

int user_thread_create(struct thread **p_thread, unsigned int flags,
		void *ip, void *sp) {
	struct ue_data *data;
	int res;
	struct thread *t;

	sched_lock();
	{
		if (!(data = pool_alloc(&ue_data_pool))) {
			sched_unlock();
			return -EAGAIN;
		}

		data->ip = ip;
		data->sp = sp;
		t = thread_create(flags, TRAMPOLINE, data);

		if ((res = ptr2err(t))) {
			pool_free(&ue_data_pool, data);
			sched_unlock();
			return res;
		}
		*p_thread = t;
	}
	sched_unlock();

	return ENOERR;
}

int user_task_create(void *ip, void *sp) {
	struct ue_data *data;
	int res;

	sched_lock();
	{
		if (!(data = pool_alloc(&ue_data_pool))) {
			sched_unlock();
			return -EAGAIN;
		}

		data->ip = ip;
		data->sp = sp;

		if ((res = new_task("", TRAMPOLINE, data)) < 0) {
			pool_free(&ue_data_pool, data);
			sched_unlock();
			return res;
		}
	}
	sched_unlock();

	return res;
}

