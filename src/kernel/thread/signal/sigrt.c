/**
 * @file
 * @brief RT signal support.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#include <kernel/thread/signal.h>

#include <assert.h>
#include <errno.h>

#include <mem/objalloc.h>
#include <util/math.h>

struct sigrt_link {
	struct slist_link pending_link;
	int sig;
	union sigval value;
};

OBJALLOC_DEF(sigrt_link_pool, struct sigrt_link,
		SIGRT_CNT);

int sigrt_raise(struct sigrt_data *sigrt_data, int sig,
		union sigval value) {
	struct sigrt_link *sigrt_el;
	struct sigrt_link *each_el;
	struct slist_link *last_link;

	assert(sigrt_data);

	if (!check_range(sig, SIGRT_MIN, SIGRT_MAX))
		return -EINVAL;

	sigrt_el = objalloc(&sigrt_link_pool);
	if (!sigrt_el)
		return -ENOMEM;

	sigrt_el->sig   = sig;
	sigrt_el->value = value;

	last_link = &sigrt_data->pending.sentinel;
	slist_foreach(each_el, &sigrt_data->pending, pending_link) {
		if (each_el->sig > sig)
			break;
		last_link = &each_el->pending_link;
	}

	slist_insert_after_link(&sigrt_el->pending_link, last_link);

	return 0;
}

