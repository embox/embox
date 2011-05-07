/**
 * @file
 * @brief Wrapper around Embox softirq with linux interface.
 *
 * @date 07.04.10
 * @author Eldar Abusalimov
 */

#include <stddef.h>

#include <linux/interrupt.h>
#include <kernel/softirq.h>

static void softirq_wrap(softirq_nr_t nr, void *data) {
	struct softirq_action *softirq_action = (struct softirq_action *) data;

	softirq_action->action(softirq_action);
}

void open_softirq(int nr, void(*action)(struct softirq_action *), void *data) {
	static struct softirq_action softirq_vec[SOFTIRQ_NRS_TOTAL];

	if (NULL != action && softirq_nr_valid(nr)) {
		softirq_vec[nr].data = data;
		softirq_vec[nr].action = action;
		softirq_install(nr, softirq_wrap, &softirq_vec[nr]);
	}
}
