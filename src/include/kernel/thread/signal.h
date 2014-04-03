/**
 * @file
 * @brief Per-thread signal data.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGNAL_H_
#define KERNEL_THREAD_SIGNAL_H_

#include <module/embox/kernel/thread/signal/sigstate_api.h>

#include <signal.h>

struct sigstate;
struct siginfoq;


__BEGIN_DECLS

extern struct sigstate *sigstate_init(struct sigstate *);

extern int sigstate_send(struct sigstate *, int, const siginfo_t *);
extern int sigstate_receive(struct sigstate *, siginfo_t *);


extern void thread_signal_handle(void);

__END_DECLS

#endif /* KERNEL_THREAD_SIGNAL_H_ */
