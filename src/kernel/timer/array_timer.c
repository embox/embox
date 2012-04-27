/**
 * @brief event/timer scheduling data structure
 *
 * @author Nikolay Malkovsky
 * @date 12.04.12
 */

#include <kernel/timer.h>
#include <lib/list.h>

/**
 * Evaluates n module k, suggesting k being the power of 2
 */
#define mod(n, k) (n & (k - 1))

const int inner_length = 1 << 7;
const int inner_log_length = 7;
const int outer_length = 1 << 7;

static list_head* inner[inner_length];
static list_head* outer[outer_length];
static uint32_t outer_size[outer_length];
static uint32_t inner_cur;
static uint32_t outer_cur;


EMBOX_UNIT_INIT(timer_strat_init);

static void timer_strat_init(void) {
	int i;

	for(i = 0; i < inner_length; ++i) {
		inner[i] = { inner[i], inner[i] };
	}

	for(i = 0; i < outer_length; ++i) {
		outer[i] = { outer[i], outer[i] };
		outer_size[i] = 0;
	}

	inner_cur = 0;
	outer_cur = 0;
}

static void timer_strat_start(struct sys_timer *ptimer) {
	if(ptimer->load > inner_length * outer_length) {
		return;
	}

	if(ptimer->load < inner_length) {
		list_add(&ptimer->lnk, inner[mod(inner_cur + ptimer->load, inner_length)]);
	} else {
		ptimer->cnt = mod(ptimer->load + inner_cur, inner_length);
		list_add(&ptimer->lnk, outer[mod(outer_cur + ((ptimer->load + inner_cur) >> inner_log_length), outer_length)]);
	}
}

static void timer_strat_sched() {
	list_head *tmp;
	sys_timer *ptimer;

	inner_cur = mod(inner_cur + 1, inner_length);

	if(inner_cur == 0) {
		outer_cur = mod(outer_cur + 1, outer_length);
		while(!list_empty(outer[outer_cur])) {
			tmp = outer[outer_cur]->next;
			list_del(tmp);
			list_add(tmp, inner[(list_entry(tmp, sys_timer, lnk))->cnt]);
		}
	}
	//TODO implement this part
	while(!list_empty(inner[inner_cur])) {
		ptimer = list_entry(inner[inner_cur]->next, sys_timer, lnk);
		ptimer->handle(ptimer, ptimer->param);
		list_del(inner[inner_cur]->next);
		timer_strat_start(ptimer);
	}
}

void timer_strat_stop(struct sys_timer *ptimer) {
	list_del(ptimer->lnk);
}


