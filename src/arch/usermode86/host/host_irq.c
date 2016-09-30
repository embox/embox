/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.12.2013
 */

#include "host_defs.h"

#include <assert.h>
#include <stddef.h>
#include <signal.h>

#define HOST_SIGMAX 31

extern void irq_entry(int irq_nr);

static HOST_FNX(int, sigaddset,
		CONCAT(sigset_t * set, int nr),
		set, nr)
static HOST_FNX(int, sigemptyset, sigset_t * set, set)
static HOST_FNX(int, sigismember,
		CONCAT(const sigset_t *set, int nr),
		set, nr)
static HOST_FNX(int, sigprocmask,
		CONCAT(int what, const sigset_t *set, sigset_t *oset),
		what, set, oset)
static HOST_FNX(int, sigaction,
		CONCAT(int sig, const struct sigaction *act, struct sigaction *oact),
		sig, act, oact)
static HOST_FNX(int, raise, int nr, nr)

static inline void host_sigfillset(sigset_t *set) {
	host_sigemptyset(set);
	host_sigaddset(set, SIGUSR1);
	host_sigaddset(set, SIGUSR2);
	host_sigaddset(set, SIGALRM);
	host_sigaddset(set, SIGIO);
}

static void host_signal_handler(int signal) {

	irq_entry(signal);
}

void host_ipl_init(void) {
	sigset_t iset, eset;
	struct sigaction sigact;
	int res, i;

	host_sigemptyset(&iset);
	res = host_sigprocmask(SIG_SETMASK, &iset, NULL);
	assert(res == 0);

	sigact.sa_handler = host_signal_handler;
	sigact.sa_mask = iset;
	sigact.sa_flags = SA_NODEFER;

	host_sigfillset(&eset);
	for (i = 1; i <= HOST_SIGMAX; i++) {
		if (!host_sigismember(&eset, i)) {
			continue;
		}

		res = host_sigaction(i, &sigact, NULL);
		assert(res == 0);
	}
}

int host_ipl_save(void) {
	sigset_t iset, oset;

	host_sigfillset(&iset);
	host_sigemptyset(&oset);
	host_sigprocmask(SIG_SETMASK, &iset, &oset);

	return !sigisemptyset(&oset);
}

void host_ipl_restore(int ipl) {
	sigset_t iset;

	if (ipl) {
		host_sigfillset(&iset);
	} else {
		host_sigemptyset(&iset);
	}

	host_sigprocmask(SIG_SETMASK, &iset, NULL);
}

void host_signal_send_self(int sig_nr) {

	host_raise(sig_nr);
}

