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

void sigstd_handle(struct sigstd_data *sigstd_data,
		struct sigaction *sig_table) {
	sigset_t *pending;
	int sig;
	void (*handler)(int sig);

	assert(sigstd_data);

	irq_lock();
	pending = &sigstd_data->pending;
	while (pending) {
		// sigstd_data->pending &= (pending - 1);  /* clear the LS bit */
		irq_unlock();

		sig = bitmap_find_first_set_bit(pending->bitmap, _SIG_TOTAL);
		assert(check_range_incl(sig, SIGSTD_MIN, SIGSTD_MAX));

		// TODO locks?
		handler = sig_table[sig + SIGSTD_MIN].sa_handler;
		assert(handler && "there must be at least a fallback handler");

		handler(sig);

		irq_lock();
	}
	irq_unlock();

}

