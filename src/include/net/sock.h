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
#include <net/socket.h>
#include <net/net.h>

typedef struct {
        spinlock_t    slock;
} socket_lock_t;

/**
 * Minimal network layer representation of sockets.
 * @param skc_family network address family
 * @param skc_state Connection state
 * @param skc_reuse %SO_REUSEADDR setting
 * @param skc_bound_dev_if bound device index if != 0
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
*/
        struct proto            *skc_prot;
};

/**
 * Network layer representation of sockets.
 * @param __sk_common shared layout with inet_timewait_sock
 * @param sk_protocol which protocol this socket belongs in this network family
 * @param sk_type socket type
 * @param sk_rcvbuf size of receive buffer in bytes
 * @param sk_lock synchronizer
 * @param sk_sndbuf size of send buffer in bytes
 * @param sk_flags %SO_LINGER (l_onoff), %SO_BROADCAST, %SO_KEEPALIVE,
 *                %SO_OOBINLINE settings, %SO_TIMESTAMPING settings
 * @param sk_receive_queue incoming packets
 * @param sk_write_queue Packet sending queue
 * @param sk_socket Identd and reporting IO signals
 * @sk_state_change: callback to indicate change in the state of the sock
 * @sk_data_ready: callback to indicate there is data to be processed
 * @sk_write_space: callback to indicate there is bf sending space available
 * @sk_error_report: callback to indicate errors (e.g. %MSG_ERRQUEUE)
 * @sk_backlog_rcv: callback to process the backlog
 * @sk_destruct: called at sock freeing time, i.e. when all refcnt == 0
 */
typedef struct sock {
	struct sock_common      __sk_common;
#define sk_family               __sk_common.skc_family
#define sk_prot                 __sk_common.skc_prot
	unsigned char		sk_protocol;
	unsigned short		sk_type;
	int			sk_rcvbuf;
	socket_lock_t           sk_lock;
	struct {
	        struct sk_buff *head;
	        struct sk_buff *tail;
	} sk_backlog;
	int			sk_sndbuf;
	unsigned long 		sk_flags;

	struct sk_buff_head     sk_receive_queue;
	struct sk_buff_head     sk_write_queue;

	struct socket           *sk_socket;
	void                    *sk_user_data;

	void (* sk_state_change) (struct sock *sk);
	void (* sk_data_ready) (struct sock *sk, int bytes);
	void (* sk_write_space) (struct sock *sk);
	void (* sk_error_report) (struct sock *sk);
	int (* sk_backlog_rcv) (struct sock *sk, sk_buff_t *pack);
	void (* sk_destruct) (struct sock *sk);
} sock_t;

/* Sock flags */
enum sock_flags {
        SOCK_DEAD,
        SOCK_DONE,
        SOCK_DESTROY,
        SOCK_BROADCAST
};

typedef struct proto {
        void (*close)(sock_t *sk, long timeout);
        int  (*connect)(sock_t *sk, sockaddr_t *uaddr, int addr_len);
        int  (*disconnect)(sock_t *sk, int flags);
        sock_t *(*accept)(sock_t *sk, int flags, int *err);
        int  (*ioctl)(struct sock *sk, int cmd, unsigned long arg);
        int  (*init)(sock_t *sk);
        int  (*setsockopt)(struct sock *sk, int level,
                            int optname, char *optval, int optlen);
        int  (*getsockopt)(struct sock *sk, int level,
                            int optname, char *optval, int *option);
        int  (*sendmsg)(struct kiocb *iocb, sock_t *sk, struct msghdr *msg, size_t len);
        int  (*recvmsg)(struct kiocb *iocb, sock_t *sk, struct msghdr *msg,
        			    size_t len, int noblock, int flags, int *addr_len);
        int  (*bind)(sock_t *sk, sockaddr_t *uaddr, int addr_len);
        int  (*backlog_rcv)(sock_t *sk, sk_buff_t *skb);
        void (*hash)(struct sock *sk);
        void (*unhash)(struct sock *sk);
        unsigned int            obj_size;
    	char			name[32];
} proto_t;

extern int proto_register(proto_t *prot, int alloc_slab);
extern void proto_unregister(proto_t *prot);

extern void sk_init(void);
extern sock_t *sk_alloc(/*struct net *net,*/ int family, gfp_t priority, proto_t *prot);
extern void sk_free(sock_t *sk);
extern sock_t *sk_clone(const sock_t *sk, const gfp_t priority);
extern int sock_setsockopt(socket_t *sock, int level, int op, char *optval, unsigned int optlen);
extern int sock_getsockopt(socket_t *sock, int level, int op, char *optval, int *optlen);
extern sk_buff_t *sock_alloc_send_skb(sock_t *sk, unsigned long size, int noblock, int *errcode);
extern int sock_queue_rcv_skb(sock_t *sk, sk_buff_t *skb);

#endif /* SOCK_H_ */
