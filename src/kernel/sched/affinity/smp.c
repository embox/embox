/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#include <framework/mod/options.h>
/* Check/set/get moved to smp.h. */
#include <kernel/sched/affinity.h>

/* The mask a schedee starts with. Zero keeps the upstream meaning -- any CPU
 * -- and anything else is a deliberate fence around a kernel not ready to be
 * entered from two cores at once. */
#define DEFAULT_MASK OPTION_GET(NUMBER, default_mask)

void sched_affinity_init(struct affinity *a) {
	a->mask = DEFAULT_MASK ? DEFAULT_MASK : SCHEDEE_AFFINITY_NONE;
}
