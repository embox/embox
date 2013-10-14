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

	sigstd_data->pending = 0;

	return sigstd_data;
}

int sigstd_raise(struct sigstd_data *sigstd_data, int sig) {
	assert(sigstd_data);

	if (!check_range(sig, SIGSTD_MIN, SIGSTD_MAX))
		return -EINVAL;

	irq_lock();
	sigstd_data->pending |= 0x1 << (sig - SIGSTD_MIN);
	irq_unlock();

	return 0;
}

void sigstd_handle(struct sigstd_table *sigstd_table,
		struct sigstd_data *sigstd_data) {
	assert(sigstd_table);
	assert(sigstd_data);

	sigset_t pending;
	int sig;
	void (*handler)(int sig);

	irq_lock();
	pending = sigstd_data->pending;
	while (pending) {
		sigstd_data->pending &= (pending - 1);  /* clear the LS bit */
		irq_unlock();

		sig = bit_ctz(pending);
		assert(check_range(sig, SIGSTD_MIN, SIGSTD_MAX));

		// TODO locks?
		handler = sigstd_table->handlers[sig + SIGSTD_MIN];
		if (handler)
			handler(sig);

		irq_lock();
	}
	irq_unlock();

}

