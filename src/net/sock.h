/**
 * \file sock.h
 *
 * \date Mar 17, 2009
 * \author anton
 */

#ifndef SOCK_H_
#define SOCK_H_

#include "net_device.h"

struct sock {
	unsigned char		sk_protocol;
	unsigned short		sk_type;
	int			sk_rcvbuf;
	int			sk_sndbuf;
	unsigned long 		sk_flags;
	net_device		*netdev;

	void (* sk_state_change) (struct sock *sk);
	void (* sk_data_ready) (struct sock *sk, int bytes);
	void (* sk_write_space) (struct sock *sk);
	void (* sk_error_report) (struct sock *sk);
	int (* sk_backlog_rcv) (struct sock *sk, net_packet*pack);
	void (* sk_destruct) (struct sock *sk);
};

#endif /* SOCK_H_ */
