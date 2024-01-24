/**
 * @file
 *
 * @date 19.05.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <kernel/irq_lock.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <mem/misc/pool.h>
#include <lib/libds/dlist.h>
#include <util/err.h>
#include <util/log.h>
#include <framework/mod/options.h>

#include "osal.h"

struct os_thread_info {
	struct {
		struct waitq wq;
		uint32_t *notif;
		uint32_t events;
	} wait_info;
	struct thread *thread;
	void (*run)(void *);
	void *arg;
	struct dlist_head link;
};

#define OS_THREAD_COUNT OPTION_GET(NUMBER, os_thread_count)
POOL_DEF(os_thread_pool, struct os_thread_info, OS_THREAD_COUNT);

static DLIST_DEFINE(os_threads_list);

static void *os_thread_run(void *arg) {
	struct os_thread_info *info = arg;

	info->run(arg);

	dlist_del(&info->link);
	pool_free(&os_thread_pool, info);

	return NULL;
}

struct os_thread_info *os_get_current_task(void) {
	struct os_thread_info *info;

	dlist_foreach_entry(info, &os_threads_list, link) {
		if (thread_self() == info->thread) {
			return info;
		}
	}
	return NULL;
}

int os_task_create(const char *name, void (*run)(void *), void *arg,
	    size_t stack_size, int priority, struct os_thread_info **info_p) {
	struct os_thread_info *info;

	(void) name;
	(void) stack_size;

	info = pool_alloc(&os_thread_pool);
	if (!info) {
		log_error("pool_alloc failed");
		return -1;
	}
	memset(info, 0, sizeof *info);

	info->thread = thread_create_with_stack(THREAD_FLAG_SUSPENDED, stack_size,
	                                        os_thread_run, info);
	if (err(info->thread)) {
		log_error("thread_create failed");
		return -1;
	}
	schedee_priority_set(&info->thread->schedee, priority);

	info->run = run;
	info->arg = arg;
	waitq_init(&info->wait_info.wq);

	dlist_head_init(&info->link);
	dlist_add_next(&info->link, &os_threads_list);

	*info_p = info;

	thread_launch(info->thread);

	return 0;
}

int os_task_notify_wait(uint32_t entry_bits, uint32_t exit_bits,
	    uint32_t *notif, int timeout) {
	struct waitq_link wl;
	int ret;
	struct os_thread_info *info = os_get_current_task();

	assert(info);

	waitq_link_init(&wl);
	waitq_wait_prepare(&info->wait_info.wq, &wl);

	info->wait_info.notif = notif;

	if (info->wait_info.events) {
		*info->wait_info.notif = info->wait_info.events;
		waitq_wakeup_all(&info->wait_info.wq);
	}

	ret = sched_wait_timeout(timeout, NULL);
	if (ret == -ETIMEDOUT) {
		log_debug("timeout");
		return -1;
	}

	waitq_wait_cleanup(&info->wait_info.wq, &wl);

	info->wait_info.events = 0;

	return 0;
}

void os_task_notify(struct os_thread_info *info, uint32_t value, uint32_t action) {
	assert(action == OS_NOTIFY_SET_BITS);

	irq_lock();
	{
		info->wait_info.events |= value;
		if (info->wait_info.notif) {
			*info->wait_info.notif = info->wait_info.events;
		}
	}
	irq_unlock();

	waitq_wakeup_all(&info->wait_info.wq);
}

struct os_queue {
	int item_size;
	int max_items;
	int items;
	struct dlist_head list;

	struct schedee *get_waiting_schedee;
};

#define OS_QUEUES_COUNT OPTION_GET(NUMBER, os_queues_count)
POOL_DEF(os_queues_pool, struct os_queue, OS_QUEUES_COUNT);

int os_queue_create(size_t item_size, size_t max_items, struct os_queue **queue_p) {
	struct os_queue *q;

	q = pool_alloc(&os_queues_pool);
	if (!q) {
		log_error("pool_alloc failed");
		return -1;
	}
	memset(q, 0, sizeof *q);

	q->item_size = item_size;
	q->max_items = max_items;

	dlist_init(&q->list);

	*queue_p = q;

	return 0;
}

int os_queue_put(struct os_queue *q, const void *item, int timeout) {
	struct dlist_head *q_item;

	(void) timeout;

	if (q->max_items == q->items) {
		log_debug("queue is full");
		return OS_QUEUE_FULL;
	}

	q_item = sysmalloc(sizeof (struct dlist_head) + q->item_size);
	if (!q_item) {
		log_error("sysmalloc failed");
		return -1;
	}

	memcpy(q_item + 1, item, q->item_size);

	dlist_head_init(q_item);
	dlist_add_next(q_item, &q->list);

	q->items++;

	if (q->get_waiting_schedee != NULL) {
		sched_wakeup(q->get_waiting_schedee);
	}

	return OS_QUEUE_OK;
}

int os_queue_get(struct os_queue *q, void *item, int timeout) {
	struct dlist_head *q_item;

	if (0 == q->items) {
		log_debug("queue is empty");

		if (timeout) {
			q->get_waiting_schedee = &thread_self()->schedee;
			SCHED_WAIT_TIMEOUT(q->items != 0, timeout);
			q->get_waiting_schedee = NULL;

			return os_queue_get(q, item, 0);
		} else {
			return OS_QUEUE_EMPTY;
		}
	}

	q_item = dlist_first(&q->list);
	memcpy(item, q_item + 1, q->item_size);

	dlist_del(q_item);

	sysfree(q_item);

	q->items--;

	return OS_QUEUE_OK;
}

int os_queue_peek(struct os_queue *q, void *item, int timeout) {
	struct dlist_head *q_item;

	(void) timeout;

	if (0 == q->items) {
		log_debug("queue is empty");
		return OS_QUEUE_EMPTY;
	}
	q_item = dlist_first(&q->list);
	memcpy(item, q_item + 1, q->item_size);

	return OS_QUEUE_OK;
}

int os_queue_messages_waiting(struct os_queue *q) {
	return q->items;
}

int os_queue_spaces_available(struct os_queue *q) {
	return q->max_items - q->items;
}

#define OS_MUTEX_COUNT OPTION_GET(NUMBER, os_mutex_count)
POOL_DEF(os_mutex_pool, struct mutex, OS_MUTEX_COUNT);

int os_mutex_create(struct mutex **m_p) {
	struct mutex *m;

	m = pool_alloc(&os_mutex_pool);
	if (!m) {
		log_error("pool_alloc failed");
		return -1;
	}
	mutex_init(m);

	*m_p = m;

	return 0;
}

#include <kernel/time/timer.h>
#include <mem/misc/pool.h>
#include "osal.h"

#define OS_TIMER_COUNT OPTION_GET(NUMBER, os_timer_count)
POOL_DEF(os_timer_pool, struct sys_timer, OS_TIMER_COUNT);

struct sys_timer *os_timer_create(int period, int reload, void *handler ) {
	struct sys_timer *t;

	t = pool_alloc(&os_timer_pool);
	if (!t) {
		log_error("pool_alloc failed");
		return NULL;
	}

	timer_init(t, reload ? TIMER_PERIODIC : TIMER_ONESHOT, handler, NULL );

	t->load = ms2jiffies(period);

	return t;
}

int os_timer_start(struct sys_timer * t ) {

	timer_start(t, ms2jiffies(t->load));

	return OS_TIMER_SUCCESS;
}

int os_timer_stop(struct sys_timer * t ) {

	timer_stop(t);

	return OS_TIMER_SUCCESS;
}

int os_timer_change_period(struct sys_timer * t, int period ) {
	timer_stop(t);
	timer_start(t, ms2jiffies(period));

	return OS_TIMER_SUCCESS;
}
