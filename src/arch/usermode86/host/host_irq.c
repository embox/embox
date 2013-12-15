/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.12.2013
 */

#include <assert.h>
#include <stddef.h>
#include <signal.h>

extern void irq_entry(int irq_nr);

#define HOST_SIGMAX 31

static inline void host_sigfillset(sigset_t *set) {
	sigemptyset(set);
	sigaddset(set, SIGUSR1);
	sigaddset(set, SIGUSR2);
	sigaddset(set, SIGALRM);
}

static void host_signal_handler(int signal) {

	irq_entry(signal);
}

void host_ipl_init(void) {
	sigset_t iset, eset;
	struct sigaction sigact;
	int res, i;

	sigemptyset(&iset);
	res = sigprocmask(SIG_SETMASK, &iset, NULL);
	assert(res == 0);

	sigact.sa_handler = host_signal_handler;
	sigact.sa_mask = iset;
	sigact.sa_flags = SA_NODEFER;

	host_sigfillset(&eset);
	for (i = 1; i <= HOST_SIGMAX; i++) {
		if (!sigismember(&eset, i)) {
			continue;
		}

		res = sigaction(i, &sigact, NULL);
		assert(res == 0);
	}
}

int host_ipl_save(void) {
	sigset_t iset, oset;

	host_sigfillset(&iset);
	sigemptyset(&oset);
	sigprocmask(SIG_SETMASK, &iset, &oset);

	return !sigisemptyset(&oset);
}

void host_ipl_restore(int ipl) {
	sigset_t iset;

	if (!ipl) {
		sigemptyset(&iset);
		sigprocmask(SIG_SETMASK, &iset, NULL);
	}
}

void host_signal_send_self(int sig_nr) {

	raise(sig_nr);
}

