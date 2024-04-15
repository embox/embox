/**
 * @file
 * @brief Std signals.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#include "sigstate.h"

#include <kernel/thread/signal.h>

#include <assert.h>
#include <errno.h>

#include <kernel/irq_lock.h>

#include <lib/libds/bit.h>
#include <util/math.h>

struct sigstate *sigstate_init(struct sigstate *sigstate) {
	assert(sigstate);

	sigemptyset(&sigstate->pending);
	siginfoq_init(&sigstate->infoq);

	return sigstate;
}

int sigstate_send(struct sigstate *sigstate, int sig, const siginfo_t *info) {
	int err = 0;

	assert(sigstate);

	if (!check_range(sig, 1, _SIG_TOTAL))
		return -EINVAL;

	irq_lock();

	if (info)
		err = siginfoq_enqueue(&sigstate->infoq, sig, info);
	if (!err)
		sigaddset(&sigstate->pending, sig);

	irq_unlock();

	return err;
}

static int sigset_first(sigset_t *set) {
	int sig;

	sig = bitmap_find_first_bit(set->bitmap, _SIG_TOTAL);
	if (sig == _SIG_TOTAL)
		sig = 0;  /* no pending signal */
	assert(check_range(sig, 0, _SIG_TOTAL));

	return sig;
}

int sigstate_receive(struct sigstate *sigstate, siginfo_t *info) {
	int sig;
	int nr_left;

	assert(sigstate && info);

	irq_lock();

	sig = sigset_first(&sigstate->pending);
	if (sig) {
		nr_left = siginfoq_dequeue(&sigstate->infoq, sig, info);
		if (nr_left <= 0)
			/* no more (or no at all) pending signals for this signo */
			sigdelset(&sigstate->pending, sig);

		if (nr_left < 0) {
			/* signal was not queued, fill in siginfo by hand */
			info->si_signo = sig;
			// TODO the rest
		}
	}

	irq_unlock();

	return sig;
}

