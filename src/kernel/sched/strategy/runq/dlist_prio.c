#include <kernel/sched.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/sched/sched_strategy.h>

void runq_item_init(runq_item_t *runq_link) {
    dlist_head_init(runq_link);
}

void runq_init(runq_t *queue) {
    for (int i = SCHED_PRIORITY_MIN; i <= SCHED_PRIORITY_MAX; i++) {
        dlist_init(&queue->list[i]);
    }
}

void runq_insert(runq_t *queue, struct schedee *schedee) {
    int prio = schedee_priority_get(schedee);
    if (prio < SCHED_PRIORITY_MIN || prio > SCHED_PRIORITY_MAX) {
        return;
    }
    dlist_add_prev(&schedee->runq_link, &queue->list[prio]);
}

void runq_remove(runq_t *queue, struct schedee *schedee) {
    int prio = schedee_priority_get(schedee);
    if (prio < SCHED_PRIORITY_MIN || prio > SCHED_PRIORITY_MAX) {
        return;
    }
    dlist_del(&schedee->runq_link);
}

struct schedee *runq_get_next(runq_t *queue) {
    for (int i = SCHED_PRIORITY_MAX; i >= SCHED_PRIORITY_MIN; i--) {
        if (!dlist_empty(&queue->list[i])) {
            return dlist_first_entry(&queue->list[i], struct schedee, runq_link);
        }
    }
    return NULL;
}

struct schedee *runq_extract(runq_t *queue) {
    struct schedee *schedee = runq_get_next(queue);
    if (schedee != NULL) {
        runq_remove(queue, schedee);
    }
    return schedee;
}
