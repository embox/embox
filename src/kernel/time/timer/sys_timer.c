/**
 * @file
 *
 * @brief
 *
 * @date 09.04.2012
 * @author Anton Bondarev
 */
#include <errno.h>

#include <embox/unit.h>
#include <mem/misc/pool.h>

#include <kernel/time/timer.h>
#include <kernel/time/time.h>
#include <kernel/sched/sched_lock.h>
#include <hal/clock.h>

static struct list_head timer_list;

POOL_DEF(timer_pool, sys_timer_t, OPTION_GET(NUMBER,timer_quantity));

int timer_init(struct sys_timer *tmr, unsigned int flags,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !tmr) {
		return -EINVAL;
	}

	tmr->state = 0;
	tmr->handle = handler;
	tmr->param = param;
	tmr->flags = flags;
	return ENOERR;
}

void timer_start(struct sys_timer *tmr, clock_t jiffies) {

	timer_stop(tmr);

	tmr->load = jiffies;
	tmr->cnt = clock_sys_ticks() + tmr->load;

	sched_lock();
	{
		timer_strat_start(tmr);
	}
	sched_unlock();
}

void timer_stop(struct sys_timer *tmr) {
	sched_lock();
	{
		if (timer_is_started(tmr)) {
			timer_strat_stop(tmr);
		}
	}
	sched_unlock();
}

int timer_init_start(struct sys_timer *tmr, unsigned int flags, clock_t jiffies,
		sys_timer_handler_t handler, void *param) {
	int err;

	if ((err = timer_init(tmr, flags, handler, param))) {
		return err;
	}

	timer_start(tmr, jiffies);

	return ENOERR;
}

int timer_init_start_msec(struct sys_timer *tmr, unsigned int flags, uint32_t msec,
		sys_timer_handler_t handler, void *param) {

	return timer_init_start(tmr, flags, ms2jiffies(msec), handler, param);
}

int timer_set(struct sys_timer **ptimer, unsigned int flags, uint32_t msec,
		sys_timer_handler_t handler, void *param) {
	int err;
	struct sys_timer *tmr;

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL;
	}

	if (NULL == (tmr = (sys_timer_t*) pool_alloc(&timer_pool))) {
		return -ENOMEM;
	}

	if ((err = timer_init_start_msec(tmr, flags, msec, handler, param))) {
		pool_free(&timer_pool, tmr);
		return err;
	}

	timer_set_preallocated(tmr);

	*ptimer = tmr;
	return ENOERR;
}

int timer_close(struct sys_timer *tmr) {
	if (NULL == tmr) {
		return -EINVAL;
	}

	timer_stop(tmr);

	if (timer_is_preallocated(tmr)) {
		timer_clear_preallocated(tmr);
		pool_free(&timer_pool, tmr);
	}

	return ENOERR;
}

/* for timer_list
 */
void init_timer (struct timer_list *timer)
{
	INIT_LIST_HEAD (&timer->entry);

	timer->expires = 0;
	timer->function = 0;
	timer->data = 0;
}

void add_timer (struct timer_list *timer)
{
	sched_lock();
	{
		unsigned long expires = timer->expires;

		struct list_head *pos;
		list_for_each (pos, &timer_list)
		{
			struct timer_list *t = list_entry (pos, struct timer_list, entry);
			if ((long) (t->expires-expires) < 0)
			{
				break;
			}
		}

		list_add (&timer->entry, pos);
	}
	sched_unlock();
}

int del_timer (struct timer_list *timer)
{
	int ret = 0;

	sched_lock();
	{
		if (timer->entry.next != &timer->entry)
		{
			list_del (&timer->entry);

			ret = 1;
		}

	}
	sched_unlock();

	if (ret)
	{
		INIT_LIST_HEAD (&timer->entry);
	}

	return ret;
}

static void periodic_handler_hdn(sys_timer_t *tmd, void *param) {
{

		unsigned long now = clock_sys_ticks();

		int cont;
		do
		{
			cont = 0;

			struct list_head *pos = timer_list.next;
			if (pos != &timer_list)
			{
				struct timer_list *t;
				sched_lock();
				{
					t = list_entry (pos, struct timer_list, entry);
				}
				sched_unlock();

				if ((long) (t->expires-now) <= 0)
				{
					sched_lock();
					{
						list_del (&t->entry);
					}
					sched_unlock();

					INIT_LIST_HEAD (&t->entry);

					(*t->function) (t->data);

					cont = 1;
				}
			}
		}
		while (cont);
	}
}

int linuxemu_init_timer (void) {
	static sys_timer_t tmr;
	INIT_LIST_HEAD (&timer_list);

	timer_init_start_msec(&tmr, TIMER_PERIODIC, 1000, periodic_handler_hdn, NULL);

	return 0;
}