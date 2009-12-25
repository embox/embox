/**
 * @file
 *
 * @date 24.12.2009
 * @author Anton Bondarev
 */
#include <asm/hardirq.h>
#include <kernel/cache.h>
#include <kernel/interrupt.h>
#include <string.h>

/* we want call this variable direct for increasing speed
 * we use macroses for this deal
 * we use massive because in future we want to use multi-core processors
 * we must use two way massive
 */
irq_cpustat_t irq_stat[1] ____cacheline_aligned = {0};

/* we must use two way massive if we want use multi-core processors */
static struct softirq_action softirq_vec[HARDIRQ_BITS] ____cacheline_aligned;

void open_softirq(int nr, void (*action)(struct softirq_action*), void *data) {
	softirq_vec[nr].action = action;
}

void softirq_init(void) {
	memset (softirq_vec, 0, sizeof(softirq_vec));
}

/*asmlinkage */void do_softirq(void) {
	int i;
	unsigned int mask = 1 << HI_SOFTIRQ;
	for (i = 0; i < HARDIRQ_BITS; i ++) {
		if (local_softirq_pending() & mask) {
			if (NULL != softirq_vec[i].action) {
				softirq_vec[i].action (softirq_vec[i].data);
			}
		}
		mask = mask << 1;
	}

}

void irq_exit(void) {
	if (local_softirq_pending())
		do_softirq();
}
