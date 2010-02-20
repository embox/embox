/**
 * @file
 *
 * @date 11.03.2009
 * @author Anton Bondarev
 */
#ifndef NET_H_
#define NET_H_

#include <types.h>
#include <linux/compiler.h>
#include <linux/aio.h>
#include <net/socket.h>

#define IPV4_ADDR_LENGTH            0x4

typedef unsigned char enet_addr_t[6];

#define NPROTO		33		/* should be enough for now..	*/

#define SYS_SOCKET	1		/* sys_socket(2)		*/
#define SYS_BIND	2		/* sys_bind(2)			*/
#define SYS_CONNECT	3		/* sys_connect(2)		*/
#define SYS_LISTEN	4		/* sys_listen(2)		*/
#define SYS_ACCEPT	5		/* sys_accept(2)		*/
#define SYS_GETSOCKNAME	6		/* sys_getsockname(2)		*/
#define SYS_GETPEERNAME	7		/* sys_getpeername(2)		*/
#define SYS_SOCKETPAIR	8		/* sys_socketpair(2)		*/
#define SYS_SEND	9		/* sys_send(2)			*/
#define SYS_RECV	10		/* sys_recv(2)			*/
#define SYS_SENDTO	11		/* sys_sendto(2)		*/
#define SYS_RECVFROM	12		/* sys_recvfrom(2)		*/
#define SYS_SHUTDOWN	13		/* sys_shutdown(2)		*/
#define SYS_SETSOCKOPT	14		/* sys_setsockopt(2)		*/
#define SYS_GETSOCKOPT	15		/* sys_getsockopt(2)		*/
#define SYS_SENDMSG	16		/* sys_sendmsg(2)		*/
#define SYS_RECVMSG	17		/* sys_recvmsg(2)		*/


typedef enum {
	SS_FREE = 0, /* not allocated                */
	SS_UNCONNECTED, /* unconnected to any socket    */
	SS_CONNECTING, /* in process of connecting     */
	SS_CONNECTED, /* connected to socket          */
	SS_DISCONNECTING
/* in process of disconnecting  */
} socket_state;

/**
 * enum sock_type - Socket types
 * @SOCK_STREAM: stream (connection) socket
 * @SOCK_DGRAM: datagram (conn.less) socket
 * @SOCK_RAW: raw socket
 * @SOCK_RDM: reliably-delivered message
 * @SOCK_SEQPACKET: sequential packet socket
 * @SOCK_DCCP: Datagram Congestion Control Protocol socket
 * @SOCK_PACKET: linux specific way of getting packets at the dev level.
 *		  For writing rarp and other similar things on the user level.
 *
 * When adding some new socket type please
 * grep ARCH_HAS_SOCKET_TYPE include/asm-* /socket.h, at least MIPS
 * overrides this enum for binary compat reasons.
 */
enum sock_type {
//	SOCK_STREAM	= 1,
	SOCK_DGRAM	= 2,
	SOCK_RAW	= 3,
//	SOCK_RDM	= 4,
//	SOCK_SEQPACKET	= 5,
//	SOCK_DCCP	= 6,
	SOCK_PACKET	= 10
};

#define SOCK_MAX (SOCK_PACKET + 1)

/**
 * General BSD socket
 * @param state socket state (%SS_CONNECTED, etc)
 * @param type socket type (%SOCK_STREAM, etc)
 * @param flags socket flags (%SOCK_ASYNC_NOSPACE, etc)
 * @param sk internal networking protocol agnostic socket representation
 * @param ops protocol specific socket operations
 */
typedef struct socket {
	socket_state state;
	short type;
	unsigned long flags;
#if 0
	struct file   *file;
#endif
	struct sock *sk;
	const struct proto_ops *ops;
} socket_t;

struct proto_ops {
	int family;
#if 0
	struct module *owner;
#endif
	int (*release)(struct socket *sock);
	int (*bind)(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len);
	int (*connect)(struct socket *sock, struct sockaddr *vaddr,
			int sockaddr_len, int flags);
	int (*socketpair)(struct socket *sock1, struct socket *sock2);
	int (*accept)(struct socket *sock, struct socket *newsock, int flags);
	int (*getname)(struct socket *sock, struct sockaddr *addr,
			int *sockaddr_len, int peer);
#if 0
	unsigned int (*poll)(struct file *file, struct socket *sock,
			struct poll_table_struct *wait);
#endif

	int (*ioctl)(struct socket *sock, unsigned int cmd, unsigned long arg);
	int (*compat_ioctl)(struct socket *sock, unsigned int cmd,
			unsigned long arg);
	int (*listen)(struct socket *sock, int len);
	int (*shutdown)(struct socket *sock, int flags);
	int (*setsockopt)(struct socket *sock, int level, int optname,
			char __user *optval, int optlen);
	int (*getsockopt)(struct socket *sock, int level, int optname,
			char __user *optval, int __user *optlen);
	int (*compat_setsockopt)(struct socket *sock, int level, int optname,
			char __user *optval, int optlen);
	int (*compat_getsockopt)(struct socket *sock, int level, int optname,
			char __user *optval, int __user *optlen);
	int (*sendmsg)(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len);
	int (*recvmsg)(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len, int flags);
#if 0
	int (*mmap)(struct file *file, struct socket *sock,
			struct vm_area_struct * vma);
	ssize_t (*sendpage)(struct socket *sock, struct page *page, int offset,
			size_t size, int flags);
#endif
};

struct net_proto_family {
	int		family;
	int		(*create)(struct socket *sock, int protocol);
	/*TODO may be define struct module for compatible*/
#if 0
	struct module	*owner;
#endif
};

extern int      sock_register(const struct net_proto_family *fam);
extern void     sock_unregister(int family);

#endif /* NET_H_ */
