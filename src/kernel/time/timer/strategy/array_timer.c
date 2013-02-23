/**
 * @file
 * @brief event/timer scheduling data structure
 *
 * @author Nikolay Malkovsky
 * @date 12.04.12
 */

#include <kernel/timer.h>
#include <linux/list.h>
#include <embox/unit.h>

#ifdef DOUBLE_ARRAY_IMPL

/**
 * Evaluates n module k, suggesting k being the power of 2
 */
#define mod(n, k) ((n) & (k - 1))

#define inner_length (1 << 7)
#define inner_log_length 7
#define outer_length (1 << 7)

static struct list_head inner[inner_length];
static struct list_head outer[outer_length];
static uint32_t outer_size[outer_length];
static uint32_t inner_cur;
static uint32_t outer_cur;


EMBOX_UNIT_INIT(timer_strat_init);

static int timer_strat_init(void) {
	uint32_t i;

	for(i = 0; i < inner_length; ++i) {
		inner[i].next = &(inner[i]);
		inner[i].prev = &(inner[i]);
	}

	for(i = 0; i < outer_length; ++i) {
		outer[i].next = &(outer[i]);
		outer[i].prev = &(outer[i]);
		outer_size[i] = 0;
	}

	inner_cur = 0;
	outer_cur = 0;
	return 0;
}

void timer_strat_start(struct sys_timer *ptimer) {
	if(ptimer->load > inner_length * outer_length) {
		return;
	}

	if(ptimer->load < inner_length) {
		list_add(&ptimer->lnk, &inner[mod(inner_cur + ptimer->load, inner_length)]);
	} else {
		ptimer->cnt = mod(ptimer->load + inner_cur, inner_length);
		list_add(&ptimer->lnk, &outer[mod(outer_cur + ((ptimer->load + inner_cur) >> inner_log_length), outer_length)]);
	}
}

void timer_strat_sched() {
	struct list_head *tmp;
	struct sys_timer *ptimer;

	inner_cur = mod(inner_cur + 1, inner_length);

	if(inner_cur == 0) {
		outer_cur = mod(outer_cur + 1, outer_length);
		while(!list_empty(&outer[outer_cur])) {
			tmp = outer[outer_cur].next;
			list_del(tmp);
			list_add(tmp, &inner[(list_entry(tmp, struct sys_timer, lnk))->cnt]);
		}
	}

	while(!list_empty(&inner[inner_cur])) {
		ptimer = list_entry(inner[inner_cur].next, struct sys_timer, lnk);
		ptimer->handle(ptimer, ptimer->param);
		list_del(inner[inner_cur].next);
		timer_strat_start(ptimer);
	}
}

void timer_strat_stop(struct sys_timer *ptimer) {
	list_del(&ptimer->lnk);
}

#endif /* DOUBLE_ARRAY_IMPL */
