/**
 * @file
 *
 * @date Nov 7, 2013
 * @author: Anton Bondarev
 */

#ifndef SOCK_STATE_H_
#define SOCK_STATE_H_



static inline void sock_set_state(struct sock *sk, enum sock_state state) {
	assert(sk != NULL);
	sk->state = state;
}

static inline enum sock_state sock_get_state(const struct sock *sk) {
	assert(sk != NULL);
	return sk->state;
}

static inline int sock_state_bound(struct sock *sk) {
	assert(sk != NULL);
	return (sk->state == SS_BOUND) || (sk->state == SS_LISTENING)
			|| (sk->state == SS_CONNECTING)
			|| (sk->state == SS_CONNECTED)
			|| (sk->state == SS_ESTABLISHED);
}

static inline int sock_state_listening(struct sock *sk) {
	assert(sk != NULL);
	return sk->state == SS_LISTENING;
}

static inline int sock_state_connecting(struct sock *sk) {
	assert(sk != NULL);
	return sk->state == SS_CONNECTING;
}

static inline int sock_state_connected(struct sock *sk) {
	assert(sk != NULL);
	return (sk->state == SS_CONNECTED)
			|| (sk->state == SS_ESTABLISHED);
}

#endif /* SOCK_STATE_H_ */
