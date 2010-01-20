/**
 * @file
 *
 * @date 24.12.2009
 * @author: Anton Bondarev
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <asm/hardirq.h>

#define set_softirq_pending(x) (local_softirq_pending() = (x))
#define or_softirq_pending(x)  (local_softirq_pending() |= (x))

/* PLEASE, avoid to allocate new softirqs, if you need not _really_ high
   frequency threaded job scheduling. For almost all the purposes
   tasklets are more than enough. F.e. all serial device BHs et
   al. should be converted to tasklets, not to softirqs.
 */

enum
{
	HI_SOFTIRQ=0,
	TIMER_SOFTIRQ,
	NET_TX_SOFTIRQ,
	NET_RX_SOFTIRQ,
	BLOCK_SOFTIRQ,
	TASKLET_SOFTIRQ,
	SCHED_SOFTIRQ
#ifdef CONFIG_HIGH_RES_TIMERS
	HRTIMER_SOFTIRQ,
#endif
};

/* softirq mask and active fields moved to irq_cpustat_t in
 * asm/hardirq.h to get better cache usage.  KAO
 */

struct softirq_action
{
	void	(*action)(struct softirq_action *);
	void	*data;
};

/*asmlinkage */void do_softirq(void);
extern void open_softirq(int nr, void (*action)(struct softirq_action*), void *data);
extern void softirq_init(void);
extern void irq_exit(void);

#define __raise_softirq_irqoff(nr) do { or_softirq_pending(1UL << (nr)); } while (0)
#if 0
extern void FASTCALL(raise_softirq_irqoff(unsigned int nr));
extern void FASTCALL(raise_softirq(unsigned int nr));
#endif

#endif /* INTERRUPT_H_ */
