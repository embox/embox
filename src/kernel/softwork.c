/**
 * @file
 * @brief Kernel worker queue backed by soft interrupts.
 *
 * @date 29.03.13
 * @author Eldar Abusalimov
 */

#include <kernel/work.h>

#include <kernel/softirq.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(work_unit_init);

static struct work_queue workq;

static void work_softirq_handler(unsigned int softirq_nr, void *data) {
	work_queue_run(&workq);
}

void softwork_post(struct work *w) {
	work_post(w, &workq);
	softirq_raise(SOFTIRQ_NR_WORK);
}

static int work_unit_init(void) {
	work_queue_init(&workq);
	return softirq_install(SOFTIRQ_NR_WORK, work_softirq_handler, NULL);
}
