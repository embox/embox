/**
 * @file
 *
 * @brief
 *
 * @date 11.04.2012
 * @author Anton Bondarev
 */

#ifndef TIMER_STRAT_H_
#define TIMER_STRAT_H_
struct sys_timer;
#include <module/embox/kernel/timer/strategy/api.h>

/********
 * timer_strat
 */
extern void timer_strat_sched(void);

extern void timer_strat_stop(struct sys_timer *ptimer);

extern void timer_strat_start(struct sys_timer *ptimer);

#endif /* TIMER_STRAT_H_ */
