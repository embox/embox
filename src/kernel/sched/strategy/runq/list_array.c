/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <lib/libds/dlist.h>

#include <kernel/task.h>
#include <kernel/sched/sched_strategy.h>

#include <module/embox/arch/smp.h>
#include <kernel/cpu/cpu.h>
#include <util/atomic_rmw.h>

struct schedee;

/* How many times a reader saw the map and the lists disagree. They are two
 * views of one state and only rq.lock makes them one: runq_remove() calls
 * dlist_del() and clears the bit afterwards, so between those two the level
 * reads as occupied and is empty.
 *
 * Counted rather than asserted: the count measures the wrong observation
 * itself instead of waiting for the crash it sometimes causes. */
unsigned long runq_map_stale;

/* How many times a schedee left the queue at a different priority than it
 * entered it -- see struct schedee::runq_prio. Not a race counter: this
 * happens on one CPU, and every one of them left the priority map describing
 * something the lists do not. */
unsigned long runq_prio_moved;

void runq_item_init(runq_item_t *runq_link) {
	dlist_head_init(runq_link);
}

/* runq operations */

void runq_init(runq_t *queue) {
	int i;

	for (i = SCHED_PRIORITY_MIN; i <= SCHED_PRIORITY_MAX; i++) {
		dlist_init(&queue->list[i]);
	}
	for (i = 0; i < RUNQ_MAP_WORDS; i++) {
		queue->occupied[i] = 0;
	}
}

/* The highest occupied priority at or below `prio`, or -1 if there is none.
 * Four loads and a clz where the loop this replaces took up to 256
 * iterations. */
static inline int runq_occupied_below(runq_t *queue, int prio) {
	uint64_t word;
	int w;

	if (prio < SCHED_PRIORITY_MIN) {
		return -1;
	}
	if (prio > SCHED_PRIORITY_MAX) {
		prio = SCHED_PRIORITY_MAX;
	}

	w = prio >> 6;
	word = queue->occupied[w];
	if ((prio & 63) != 63) {
		word &= (1ULL << ((prio & 63) + 1)) - 1;
	}

	while (1) {
		if (word) {
			return (w << 6) + 63 - __builtin_clzll(word);
		}
		if (w == 0) {
			return -1;
		}
		word = queue->occupied[--w];
	}
}

void runq_insert(runq_t *queue, struct schedee *schedee) {
	int prio = schedee_priority_get(schedee);

	/* Remember where it went, because the answer to "what priority is this"
	 * can change while it is in there. */
	schedee->runq_prio = prio;
	dlist_add_prev(&schedee->runq_link, &queue->list[prio]);
	/* Set unconditionally -- the list is non-empty by construction, and a
	 * redundant store is cheaper than a test. */
	queue->occupied[prio >> 6] |= 1ULL << (prio & 63);
}

void runq_remove(runq_t *queue, struct schedee *schedee) {
	/* The level it is in, not the priority it now claims. dlist_del()
	 * unlinks it from wherever it really is either way; the bit that has to
	 * be reconsidered belongs to that list and no other. */
	int prio = schedee->runq_prio;

	if (prio != schedee_priority_get(schedee)) {
		atomic_rmw_add_fetch(&runq_prio_moved, 1, __ATOMIC_RELAXED);
	}

	dlist_del(&schedee->runq_link);
	/* Cleared only when the level is observed empty, so the bit can never
	 * be clear while something is queued. */
	if (dlist_empty(&queue->list[prio])) {
		queue->occupied[prio >> 6] &= ~(1ULL << (prio & 63));
	}
}

struct schedee *runq_get_next(runq_t *queue) {
	const unsigned int mask = 1 << cpu_get_id();
	struct schedee *schedee = NULL;
	int i;

	/* Only occupied levels are visited. An occupied one is still walked -- the
	 * affinity of its members decides. */
	for (i = runq_occupied_below(queue, SCHED_PRIORITY_MAX); i >= 0;
	     i = runq_occupied_below(queue, i - 1)) {
		struct schedee *s;

		dlist_foreach_entry(s, &queue->list[i], runq_link) {
			/* Checking the affinity */

			if (sched_affinity_check(&s->affinity, mask)) {
				schedee = s;
				break;
			}
		}

		if (schedee) {
			break;
		}
	}

	return schedee;
}

struct schedee *runq_extract(runq_t *queue) {
	const unsigned int mask = 1 << cpu_get_id();
	struct schedee *schedee = NULL;
	int i;

	for (i = runq_occupied_below(queue, SCHED_PRIORITY_MAX); i >= 0;
	     i = runq_occupied_below(queue, i - 1)) {
		struct schedee *s;

		dlist_foreach_entry(s, &queue->list[i], runq_link) {
			/* Checking the affinity */

			if (sched_affinity_check(&s->affinity, mask)) {
				schedee = s;
				break;
			}
		}

		if (schedee) {
			runq_remove(queue, schedee);
			break;
		}
	}

	return schedee;
}

struct schedee *runq_get_next_ignore_affinity(runq_t *queue) {
	/* With no affinity to check, the top occupied level is the answer
	 * outright. */
	int i = runq_occupied_below(queue, SCHED_PRIORITY_MAX);

	if (i < 0) {
		return NULL;
	}

	/* And the map is only as fresh as the lock the caller holds.
	 * dlist_first_entry() on an empty list asserts. */
	if (dlist_empty(&queue->list[i])) {
		atomic_rmw_add_fetch(&runq_map_stale, 1, __ATOMIC_RELAXED);
		return NULL;
	}

	return dlist_first_entry(&queue->list[i], struct schedee, runq_link);
}
