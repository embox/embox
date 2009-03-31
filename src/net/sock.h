/*
 * sock.h
 *
 *  Created on: Mar 17, 2009
 *      Author: anton
 */

#ifndef SOCK_H_
#define SOCK_H_

#include "net_device.h"

typedef enum _sk_type
{
	SOCK_STREAM, SOCK_DGRAM, SOCK_RAW
}sk_type;
typedef enum _sk_proto
{
	TCP, UDP, IPPROTO_RAW
}sk_proto;
struct sock {
	unsigned char		sk_protocol;
	unsigned short		sk_type;
	int			sk_rcvbuf;
	int			sk_sndbuf;
	unsigned long 		sk_flags;
	net_device *netdev;

	void (* sk_state_change) (struct sock *sk);
	void (* sk_data_ready) (struct sock *sk, int bytes);
	void (* sk_write_space) (struct sock *sk);
	void (* sk_error_report) (struct sock *sk);
	int (* sk_backlog_rcv) (struct sock *sk, net_packet*pack);
	void (* sk_destruct) (struct sock *sk);
};
#endif /* SOCK_H_ */
