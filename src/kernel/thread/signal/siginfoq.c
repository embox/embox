/**
 * @file
 * @brief Signal queueing support.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#include <kernel/thread/signal.h>

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <mem/objalloc.h>
#include <util/math.h>

#include <framework/mod/options.h>

#define SIGINFO_POOL_SZ \
	OPTION_GET(NUMBER, siginfo_pool_sz)

struct siginfoq_link {
	struct slist_link link;
	siginfo_t info;
};
typedef member_t(struct siginfoq_link, link) siginfoq_member_t;

OBJALLOC_DEF(siginfoq_link_pool, struct siginfoq_link, SIGINFO_POOL_SZ);

struct siginfoq *siginfoq_init(struct siginfoq *infoq) {
	slist_init(&infoq->queue);
	infoq->last = &infoq->queue.sentinel;

	return infoq;
}

int siginfoq_enqueue(struct siginfoq *infoq, int sig,
		const siginfo_t *info) {
	struct siginfoq_link *infoq_el;

	assert(info);

	infoq_el = objalloc(&siginfoq_link_pool);
	if (!infoq_el)
		return -ENOMEM;

	memcpy(&infoq_el->info, info, sizeof(*info));
	infoq_el->info.si_signo = sig;

	slist_insert_after_link(&infoq_el->link, infoq->last);
	infoq->last = &infoq_el->link;

	return 0;
}

int siginfoq_dequeue(struct siginfoq *infoq, int sig, siginfo_t *info) {
	struct slist *queue = &infoq->queue;
	struct siginfoq_link *first_el = NULL;
	struct siginfoq_link *each_el;
	struct slist_link *last_link;
	int still_pending = 0;

	assert(info);

	last_link = &queue->sentinel;
	slist_foreach(each_el, queue, link) {
		if (each_el->info.si_signo == sig) {
			if (first_el) {
				still_pending = 1;
				break;
			}
			first_el = each_el;

		} else if (!first_el) {
			last_link = &each_el->link;
		}
	}

	if (!first_el)
		return -ENOENT;

	// XXX need slist method to unlink an element -- Eldar
	assert(last_link->next == &first_el->link);
	last_link->next = first_el->link.next;
	if (infoq->last == &first_el->link)
		infoq->last = last_link;
	assert(infoq->last->next == &queue->sentinel);

	memcpy(info, &first_el->info, sizeof(*info));
	objfree(&siginfoq_link_pool, first_el);

	return still_pending;  /* whether more signals still remain in queue */
}
