/**
 * @file
 *
 * @date 25.12.2009
 * @author Anton Bondarev
 */

#ifndef HARDIRQ_H_
#define HARDIRQ_H_

#include <kernel/cache.h>

typedef struct irq_cpustat {
	unsigned int __softirq_pending;
} ____cacheline_aligned irq_cpustat_t;

#include <kernel/irq_cpustat.h>

#define HARDIRQ_BITS    8

#endif /* HARDIRQ_H_ */
