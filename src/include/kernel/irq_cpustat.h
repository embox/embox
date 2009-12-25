/**
 * @file
 *
 * @date 25.12.2009
 * @author Anton Bondarev
 */

#ifndef IRQ_CPUSTAT_H_
#define IRQ_CPUSTAT_H_


extern irq_cpustat_t irq_stat[1];		/* defined in asm/hardirq.h */

  /* arch independent irq_stat fields */
#define local_softirq_pending() \
		(irq_stat[0].__softirq_pending)

#endif /* IRQ_CPUSTAT_H_ */
