/**
 * @file
 * @brief Simple implementation of timer scheduler using single array
 *
 * @author Nikolay Malkovsky
 * @date 12.04.12
 */

#include <kernel/timer.h>
#include <linux/list.h>
#include <embox/unit.h>

#ifdef SINGLE_ARRAY_IMPL

/**
 * Evaluates n module k, suggesting k being the power of 2
 */
#define mod(n, k) ((n) & (k - 1))

#define length (1 << 10)

static struct list_head timeline[length];
static uint32_t cur;

EMBOX_UNIT_INIT(timer_strat_init);

static int timer_strat_init(void) {
	size_t i;

	for (i = 0; i < length; ++i) {
		timeline[i].next = &(timeline[i]);
		timeline[i].prev = &(timeline[i]);
	}

	cur = 0;
	return 0;
}

void timer_strat_start(struct sys_timer *ptimer) {
	if (ptimer->load > length) {
		return;
	}
	list_add(&ptimer->lnk, &timeline[mod(ptimer->load + cur, length)]);
}

void timer_strat_sched() {
	struct sys_timer *ptimer;
	cur = mod(cur + 1, length);

	while (!list_empty(&timeline[cur])) {
		ptimer = list_entry(timeline[cur].next, struct sys_timer, lnk);

		ptimer->handle(ptimer, ptimer->param);
		list_del(timeline[cur].next);
		list_add(&ptimer->lnk, &timeline[mod(ptimer->load + cur, length)]);
	}
}

void timer_strat_stop(struct sys_timer *ptimer) {
	list_del(&ptimer->lnk);
}

#endif /* SINGLE_ARRAY_IMPL */
