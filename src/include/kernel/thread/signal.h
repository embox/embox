/**
 * @file
 * @brief Per-thread signal data.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGNAL_H_
#define KERNEL_THREAD_SIGNAL_H_

#include <module/embox/kernel/thread/signal/sigstd_api.h>
#include <module/embox/kernel/thread/signal/sigrt_api.h>

#include <signal.h>

/* defined above */
struct sigstd_data;
struct sigrt_data;

struct signal_data {
	struct sigstd_data sigstd_data;
	struct sigrt_data  sigrt_data;
};


extern struct sigstd_data *sigstd_data_init(struct sigstd_data *);

extern int sigstd_raise(struct sigstd_data *, int);
extern int sigstd_dequeue(struct sigstd_data *);

extern struct sigrt_data *sigrt_data_init(struct sigrt_data *);

extern int sigrt_raise(struct sigrt_data *, int, union sigval);
extern int sigrt_dequeue(struct sigrt_data *);


extern void thread_signal_handle(void);

#endif /* KERNEL_THREAD_SIGNAL_H_ */
