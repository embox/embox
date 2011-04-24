/**
 * @file
 *
 * @date 19.03.2009
 * @author Anton Bondarev
 * @author Nikolay Korotky
 */
#ifndef SOCKET_H_
#define SOCKET_H_

#include <net/skbuff.h>

/* Supported address families. */
#define AF_UNSPEC	0
//#define AF_UNIX		1	/* Unix domain sockets 		*/
//#define AF_LOCAL	1	/* POSIX name for AF_UNIX	*/
#define AF_INET		2	/* Internet IP Protocol 	*/
//#define AF_AX25		3	/* Amateur Radio AX.25 		*/
//#define AF_IPX		4	/* Novell IPX 			*/
//#define AF_APPLETALK	5	/* AppleTalk DDP 		*/
//#define AF_NETROM	6	/* Amateur Radio NET/ROM 	*/
//#define AF_BRIDGE	7	/* Multiprotocol bridge 	*/
//#define AF_ATMPVC	8	/* ATM PVCs			*/
//#define AF_X25		9	/* Reserved for X.25 project 	*/
//#define AF_INET6	10	/* IP version 6			*/
//#define AF_ROSE		11	/* Amateur Radio X.25 PLP	*/
//#define AF_DECnet	12	/* Reserved for DECnet project	*/
//#define AF_NETBEUI	13	/* Reserved for 802.2LLC project*/
//#define AF_SECURITY	14	/* Security callback pseudo AF */
//#define AF_KEY		15      /* PF_KEY key management API */
//#define AF_NETLINK	16
//#define AF_ROUTE	AF_NETLINK /* Alias to emulate 4.4BSD */
#define AF_PACKET	17	/* Packet family		*/
//#define AF_ASH		18	/* Ash				*/
//#define AF_ECONET	19	/* Acorn Econet			*/
//#define AF_ATMSVC	20	/* ATM SVCs			*/
//#define AF_SNA		22	/* Linux SNA Project (nutters!) */
//#define AF_IRDA		23	/* IRDA sockets			*/
//#define AF_PPPOX	24	/* PPPoX sockets		*/
//#define AF_WANPIPE	25	/* Wanpipe API Sockets */
//#define AF_LLC		26	/* Linux LLC			*/
//#define AF_TIPC		30	/* TIPC sockets			*/
//#define AF_BLUETOOTH	31	/* Bluetooth sockets 		*/
//#define AF_IUCV		32	/* IUCV sockets			*/
#define AF_MAX		33	/* For now.. */

/* Protocol families, same as address families. */
#define PF_UNSPEC	AF_UNSPEC
//#define PF_UNIX		AF_UNIX
//#define PF_LOCAL	AF_LOCAL
#define PF_INET		AF_INET
//#define PF_AX25		AF_AX25
//#define PF_IPX		AF_IPX
//#define PF_APPLETALK	AF_APPLETALK
//#define	PF_NETROM	AF_NETROM
//#define PF_BRIDGE	AF_BRIDGE
//#define PF_ATMPVC	AF_ATMPVC
//#define PF_X25		AF_X25
//#define PF_INET6	AF_INET6
//#define PF_ROSE		AF_ROSE
//#define PF_DECnet	AF_DECnet
//#define PF_NETBEUI	AF_NETBEUI
//#define PF_SECURITY	AF_SECURITY
//#define PF_KEY		AF_KEY
//#define PF_NETLINK	AF_NETLINK
//#define PF_ROUTE	AF_ROUTE
#define PF_PACKET	AF_PACKET
//#define PF_ASH		AF_ASH
//#define PF_ECONET	AF_ECONET
//#define PF_ATMSVC	AF_ATMSVC
//#define PF_SNA		AF_SNA
//#define PF_IRDA		AF_IRDA
//#define PF_PPPOX	AF_PPPOX
//#define PF_WANPIPE	AF_WANPIPE
//#define PF_LLC		AF_LLC
//#define PF_TIPC		AF_TIPC
//#define PF_BLUETOOTH	AF_BLUETOOTH
//#define PF_IUCV		AF_IUCV
#define PF_MAX		AF_MAX

/**
 * Define info about protocol families
 * @param id - identifer of protocol family
 * @return string info
 */
static inline char* trace_pf_info(int id) {
	switch(id) {
		case PF_INET:
			return "Internet IP Protocol";
		case PF_PACKET:
			return "Packet family";
	}
	return "";
}

/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	128

/* Flags we can use with send/ and recv.
   Added those for 1003.1g not all are supported yet
 */
#define MSG_OOB		1
#define MSG_PEEK	2
#define MSG_DONTROUTE	4
#define MSG_TRYHARD     4       /* Synonym for MSG_DONTROUTE for DECnet */
#define MSG_CTRUNC	8
#define MSG_PROBE	0x10	/* Do not send. Only probe path f.e. for MTU */
#define MSG_TRUNC	0x20
#define MSG_DONTWAIT	0x40	/* Nonblocking io		 */
#define MSG_EOR         0x80	/* End of record */
#define MSG_WAITALL	0x100	/* Wait for a full request */
#define MSG_FIN         0x200
#define MSG_SYN		0x400
#define MSG_CONFIRM	0x800	/* Confirm path validity */
#define MSG_RST		0x1000
#define MSG_ERRQUEUE	0x2000	/* Fetch message from error queue */
#define MSG_NOSIGNAL	0x4000	/* Do not generate SIGPIPE */
#define MSG_MORE	0x8000	/* Sender will send more */

#define MSG_EOF         MSG_FIN

#if defined(CONFIG_COMPAT)
#define MSG_CMSG_COMPAT	0x80000000	/* This message needs 32 bit fixups */
#else
#define MSG_CMSG_COMPAT	0		/* We never have 32 bit fixups */
#endif


/* Setsockoptions(2) level. Thanks to BSD these must match IPPROTO_xxx */
#define SOL_IP		0
/* #define SOL_ICMP	1	No-no-no! Due to Linux :-) we cannot use SOL_ICMP=1 */
//#define SOL_TCP		6
#define SOL_UDP		17
//#define SOL_IPV6	41
//#define SOL_ICMPV6	58
//#define SOL_SCTP	132
//#define SOL_UDPLITE	136     /* UDP-Lite (RFC 3828) */
#define SOL_RAW		255
//#define SOL_IPX		256
//#define SOL_AX25	257
//#define SOL_ATALK	258
//#define SOL_NETROM	259
//#define SOL_ROSE	260
//#define SOL_DECNET	261
//#define	SOL_X25		262
//#define SOL_PACKET	263
//#define SOL_ATM		264	/* ATM layer (cell level) */
//#define SOL_AAL		265	/* ATM Adaption Layer (packet level) */
//#define SOL_IRDA        266
//#define SOL_NETBEUI	267
//#define SOL_LLC		268
//#define SOL_DCCP	269
//#define SOL_NETLINK	270
//#define SOL_TIPC	271

/* IPX options */
#define IPX_TYPE	1

struct iovec {
	void  *iov_base;
	size_t iov_len;
};

struct msghdr {
	void	*	msg_name;	/* Socket name			*/
	int		msg_namelen;	/* Length of name		*/
	struct iovec *	msg_iov;	/* Data blocks			*/
#if 0
	__kernel_size_t	msg_iovlen;	/* Number of blocks		*/
#endif
	void 	*	msg_control;	/* Per protocol magic (eg BSD file descriptor passing) */
#if 0
	__kernel_size_t	msg_controllen;	/* Length of cmsg list */
#endif
	unsigned	msg_flags;
};

typedef unsigned short  sa_family_t;

typedef struct sockaddr {
	sa_family_t    sa_family;    /* address family, AF_xxx */
	char           sa_data[14];  /* 14 bytes of protocol address */
} sockaddr_t;

/*TODO: move out of here*/
extern int sock_init(void);

/**
 * create an endpoint for communication.
 * @param protocol The protocol specifies a particular protocol to be used with the socket.
 * 	Normally only a single protocol exists to support a particular socket type within a
 * 	given protocol family, in which case protocol can be specified as 0.
 * @return On success, a file descriptor for the new socket is returned.
 * 	On error, -1 is returned.
 */
extern int socket(int domain, int type, int protocol);

/**
 * setup of connection
 */
extern int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/**
 * bind a name to a socket.
 * @return 0 on success.  On error, -1.
 */
extern int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * send a message on a socket.
 * @return the number of characters sent. On error, -1.
 */
extern ssize_t sendto(int sockfd, const void *buf, int len, int flags,
		const struct sockaddr *dest_addr, socklen_t addrlen);

/**
 * receive a message from a socket.
 * @return the number of bytes received, or -1 if an error occurred.
 */
extern ssize_t recvfrom(int sockfd, void *buf, int len, int flags,
		struct sockaddr *src_addr, socklen_t *addrlen);

/**
 * check message in a socket
 * @return 1 if there isn't a message in socket, 0 otherwise or -1 if an error occurred.
 */
extern int empty_socket(int sockfd);

/**
 * close a socket descriptor
 * @return 0 on success. On error, -1.
 */
extern int close(int sockfd);

#endif /* SOCKET_H_ */
