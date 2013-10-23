/**
 * @file
 * @brief Std signals.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#include <kernel/thread/signal.h>

#include <assert.h>
#include <errno.h>

#include <kernel/irq_lock.h>

#include <util/bit.h>
#include <util/math.h>

struct sigstd_data * sigstd_data_init(
		struct sigstd_data *sigstd_data) {
	assert(sigstd_data);

	sigemptyset(&sigstd_data->pending);

	return sigstd_data;
}

int sigstd_raise(struct sigstd_data *sigstd_data, int sig) {
	assert(sigstd_data);

	return IRQ_LOCKED_DO(sigaddset(&sigstd_data->pending, sig));
}

static int sigset_first(sigset_t *set) {
	int sig;

	sig = bitmap_find_first_bit(set->bitmap, _SIG_TOTAL);
	if (sig >= _SIG_TOTAL)
		sig = 0;
	assert(check_range(sig, 0, _SIG_TOTAL));

	return sig;
}

int sigstd_dequeue(struct sigstd_data *sig_data) {
	int sig;

	assert(sig_data);

	irq_lock();

	sig = sigset_first(&sig_data->pending);
	if (sig)
		sigdelset(&sig_data->pending, sig);

	irq_unlock();

	return sig;
}

