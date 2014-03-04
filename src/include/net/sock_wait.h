/*
 * @file
 *
 * @date Dec 5, 2013
 * @author: Anton Bondarev
 */

#ifndef SOCK_WAIT_H_
#define SOCK_WAIT_H_

extern int sock_wait(struct sock *sk, int flags, int timeout);

extern void sock_wait_lock(void);

extern void sock_wait_unlock(void);

#endif /* SOCK_WAIT_H_ */
