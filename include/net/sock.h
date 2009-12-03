/**
 * @file sock.h
 *
 * @brief Definitions for the AF_INET socket handler.
 * @date 17.03.2009
 * @author Anton Bondarev
 */

#ifndef SOCK_H_
#define SOCK_H_

#include <net/skbuff.h>
#include <net/netdevice.h>

#define MAX_SOCK_NUM 4

struct udp_sock;

/**
 * struct sock_common - minimal network layer representation of sockets
 */
struct sock_common {
        unsigned short          skc_family;
        volatile unsigned char  skc_state;
        unsigned char           skc_reuse;
        int                     skc_bound_dev_if;
/*        struct hlist_node       skc_node;
        struct hlist_node       skc_bind_node;
        atomic_t                skc_refcnt;
        unsigned int            skc_hash;
        struct proto            *skc_prot;
*/
};

/**
 * struct sock - network layer representation of sockets
 * @param sk_type socket type
 * @param sk_rcvbuf size of receive buffer in bytes
 * @param sk_sndbuf size of send buffer in bytes
 */
struct sock {
	struct sock_common      __sk_common;
	unsigned char		sk_protocol;
	unsigned short		sk_type;
	int			sk_rcvbuf;
	int			sk_sndbuf;
	unsigned long 		sk_flags;

	struct sk_buff_head     sk_receive_queue;
	struct sk_buff_head     sk_write_queue;

	struct socket           *sk_socket;
	struct net_device	*netdev;

	void (* sk_state_change) (struct sock *sk);
	void (* sk_data_ready) (struct sock *sk, int bytes);
	void (* sk_write_space) (struct sock *sk);
	void (* sk_error_report) (struct sock *sk);
	int (* sk_backlog_rcv) (struct sock *sk, struct sk_buff *pack);
	void (* sk_destruct) (struct sock *sk);
};

typedef struct _SOCK_INFO{
        struct udp_sock *sk;
        struct sk_buff *queue; //TODO: stub
        int new_pack;
        int is_busy;
}SOCK_INFO;

extern SOCK_INFO sks[MAX_SOCK_NUM];

/**
 * All socket objects are allocated here
 */
extern struct udp_sock* sk_alloc();
extern void sk_free(struct udp_sock *sk);

#endif /* SOCK_H_ */
