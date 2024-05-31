/**
 * @file
 * @brief POSIX socket API
 *
 * @date 01.02.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_SYS_SOCKET_H_
#define COMPAT_POSIX_SYS_SOCKET_H_

#include <sys/cdefs.h>
#include <sys/uio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

typedef int32_t socklen_t;

typedef unsigned short sa_family_t;

typedef struct sockaddr {
	sa_family_t sa_family; /* Address family, AF_xxx */
	char sa_data[14];      /* 14 bytes of protocol address */
} sockaddr_t;

struct sockaddr_storage {
	sa_family_t ss_family;
	char __ss_storage[100];
};

struct iovec;

struct msghdr {
	void *msg_name;              /* optional address*/
	socklen_t msg_namelen;       /* size of address */
	struct iovec *msg_iov;       /* scatter/gather array */
	int           msg_iovlen;    /* members in msg_iov */
	void         *msg_control;   /* ancillary data, see below */
	socklen_t     msg_controllen;/* ancillary data buffer len */
	int           msg_flags;     /* flags on received message */
};


struct cmsghdr {
	socklen_t     cmsg_len;       /* data byte count, including the cmsghdr */
	int           cmsg_level;     /* originating protocol */
	int           cmsg_type;      /* protocol-specific type */
};

struct linger {
	int         l_onoff;          /* indicates whether linger option is enabled */
	int         l_linger;         /* linger time, in seconds */
};

#define SCM_RIGHTS     0x1

#define __CMSG_ALIGN(len) \
	(((len) + sizeof(long) - 1) & ~(sizeof(long) - 1))

#define CMSG_FIRSTHDR(mhdr) \
	((mhdr)->msg_controllen >= sizeof(struct cmsghdr) ? \
		(struct cmsghdr *) (mhdr)->msg_control : \
		(struct cmsghdr *) NULL)

#define CMSG_NXTHDR(mhdr, cmsg) \
	(((cmsg) == NULL) \
	    ? CMSG_FIRSTHDR(mhdr) \
	    : (((char *) (cmsg) + __CMSG_ALIGN((cmsg)->cmsg_len) \
	            + sizeof(struct cmsghdr) \
	            > (char *) ((mhdr)->msg_control) + (mhdr)->msg_controllen) \
	        ? (struct cmsghdr *) NULL \
	        : (struct cmsghdr *) ((char *) (cmsg) + __CMSG_ALIGN((cmsg)->cmsg_len))))

#define CMSG_DATA(cmsg) \
	((void *)((char *)(cmsg) + sizeof(struct cmsghdr)))

/*
 * CMSG_SPACE and CMSG_LEN are not POSIX, but they are part of rfc2292:
 *   https://tools.ietf.org/html/rfc2292#section-4.3
 * Since they are not Linux-specific macros and often are used alongside
 * with another CMSG_ macros we also support them.
 */
#define CMSG_SPACE(len) \
	(sizeof(struct cmsghdr) + __CMSG_ALIGN(len))

#define CMSG_LEN(len) \
	(sizeof(struct cmsghdr) + (len))

/**
 * Supported protocol/address families
 */
enum {
	PF_UNSPEC =  0, /* unspecified */
#define AF_UNSPEC PF_UNSPEC
	PF_UNIX   =  1, /* local to host (pipes and file-domain) */
#define AF_UNIX   PF_UNIX
#define PF_LOCAL  PF_UNIX /* POSIX name for PF_UNIX */
#define AF_LOCAL  PF_LOCAL
#define PF_FILE   PF_UNIX /* another name for PF_LOCAL */
#define AF_FILE   PF_FILE
	PF_INET   =  2, /* IP protocol family */
#define AF_INET   PF_INET
	PF_INET6  = 10, /* IPv6 protocol family */
#define AF_INET6  PF_INET6
	PF_PACKET = 17, /* packet family */
#define AF_PACKET PF_PACKET
	PF_NETLINK = 18,
#define AF_NETLINK PF_NETLINK
	PF_RDS     =  19,   /* Reliable Datagram Sockets (RDS) protocol */
#define AF_RDS     PF_RDS
	PF_MAX          /* upper bound of protocol family */
#define AF_MAX    PF_MAX
};

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
/**
 * Socket types
 */
enum {
	SOCK_STREAM    =  1, /* byte-stream socket */
	SOCK_DGRAM     =  2, /* datagram socket */
	SOCK_RAW       =  3, /* raw protocol socket */
	SOCK_SEQPACKET =  5, /* sequential packet socket */
	SOCK_PACKET    = 10, /* packet socket */
	SOCK_RDM       = 20, /* a reliable datagram layer */
	SOCK_TYPE_MAX        /* i.e. SOCK_PACKET + 1 */
};


/* POSIX descriptions
SO_ACCEPTCONN    Socket is accepting connections.
SO_BROADCAST    Transmission of broadcast messages is supported.
SO_DEBUG    Debugging information is being recorded.
SO_DONTROUTE    bypass normal routing
SO_ERROR    Socket error status.
SO_KEEPALIVE    Connections are kept alive with periodic messages.
SO_LINGER    Socket lingers on close.
SO_OOBINLINE    Out-of-band data is transmitted in line.
SO_RCVBUF    Receive buffer size.
SO_RCVLOWAT    receive "low water mark"
SO_RCVTIMEO    receive timeout
SO_REUSEADDR    Reuse of local addresses is supported.
SO_SNDBUF    Send buffer size.
SO_SNDLOWAT    send "low water mark"
SO_SNDTIMEO    send timeout
SO_TYPE    Socket type.
*/
/**
 * @param SO_ACCEPTCONN int Non-zero indicates that socket listening is enabled ( getsockopt() only).
 * @param SO_BROADCAST int Non-zero requests permission to transmit broadcast datagrams (SOCK_DGRAM sockets only).
 * @param SO_DEBUG int Non-zero requests debugging in underlying protocol modules.
 * @param SO_DONTROUTE int Non-zero requests bypass of normal routing; route based on destination address only.
 * @param SO_ERROR int Requests and clears pending error information on the socket ( getsockopt() only).
 * @param SO_KEEPALIVE int Non-zero requests periodic transmission of keepalive messages (protocol-specific).
 * @param SO_LINGER struct linger Specify actions to be taken for queued, unsent data on close(): linger on/off and linger time in seconds.
 * @param SO_OOBINLINE int Non-zero requests that out-of-band data be placed into normal data input queue as received.
 * @param SO_RCVBUF int Size of receive buffer (in bytes).
 * @param SO_RCVLOWAT int Minimum amount of data to return to application for input operations (in bytes).
 * @param SO_RCVTIMEO struct timeval Timeout value for a socket receive operation.
 * @param SO_REUSEADDR int Non-zero requests reuse of local addresses in bind() (protocol-specific).
 * @param SO_SNDBUF int Size of send buffer (in bytes).
 * @param SO_SNDLOWAT int Minimum amount of data to send for output operations (in bytes).
 * @param SO_SNDTIMEO struct timeval Timeout value for a socket send operation.
 * @param SO_TYPE int Identify socket type ( getsockopt() only).
 */
/* enum socket_options{ */
#define SO_ACCEPTCONN   0/* int *//* Non-zero indicates that socket listening is enabled ( getsockopt() only). */
#define SO_BROADCAST    1/* int *//* Non-zero requests permission to transmit broadcast datagrams (SOCK_DGRAM sockets only). */
#define SO_DEBUG        2/* int *//* Non-zero requests debugging in underlying protocol modules. */
#define SO_DONTROUTE    3/* int *//* Non-zero requests bypass of normal routing; route based on destination address only. */
#define SO_ERROR        4/* int *//* Requests and clears pending error information on the socket ( getsockopt() only). */
#define SO_KEEPALIVE    5/* int *//* Non-zero requests periodic transmission of keepalive messages (protocol-specific). */
#define SO_LINGER       6/* struct linger *//* Specify actions to be taken for queued, unsent data on close(): linger on/off and linger time in seconds. */
#define SO_OOBINLINE    7/* int *//* Non-zero requests that out-of-band data be placed into normal data input queue as received. */
#define SO_RCVBUF       8/* int *//* Size of receive buffer (in bytes). */
#define SO_RCVLOWAT     9/* int *//* Minimum amount of data to return to application for input operations (in bytes). */
#define SO_RCVTIMEO     10/* struct timeval *//* Timeout value for a socket receive operation. */
#define SO_REUSEADDR    11/* int *//* Non-zero requests reuse of local addresses in bind() (protocol-specific). */
#define SO_SNDBUF       12/* int *//* Size of send buffer (in bytes). */
#define SO_SNDLOWAT     13/* int *//* Minimum amount of data to send for output operations (in bytes). */
#define SO_SNDTIMEO     14/* struct timeval *//* Timeout value for a socket send operation. */
#define SO_TYPE         15/* int *//* Identify socket type ( getsockopt() only). */
#define SO_BINDTODEVICE 16/* Bind socket to send packet from specified device */
#define SO_DOMAIN       17 /* int */ /* Socket domain */
#define SO_PROTOCOL     18 /* int */ /* Socket protocol */
#define SO_POSIX_MAX    19
/* }; */


/* POSIX descriptions
MSG_CTRUNC    Control data truncated.
MSG_DONTROUTE    Send without using routing tables.
MSG_EOR    Terminates a record (if supported by the protocol).
MSG_OOB    Out-of-band data.
MSG_PEEK    Leave received data in queue.
MSG_TRUNC    Normal data truncated.
MSG_WAITALL    Wait for complete message.
*/
/* Flags we can use with send/ and recv.
   Added those for 1003.1g not all are supported yet
 */
#define MSG_OOB       0x0001
#define MSG_PEEK      0x0002
#define MSG_DONTROUTE 0x0004
#define MSG_TRYHARD   0x0004 /* Synonym for MSG_DONTROUTE for DECnet */
#define MSG_CTRUNC    0x0008
#define MSG_PROBE     0x0010 /* Do not send. Only probe path f.e. for MTU */
#define MSG_TRUNC     0x0020
#define MSG_DONTWAIT  0x0040 /* Nonblocking io */
#define MSG_EOR       0x0080 /* End of record */
#define MSG_WAITALL   0x0100 /* Wait for a full request */
#define MSG_FIN       0x0200
#define MSG_SYN       0x0400
#define MSG_CONFIRM   0x0800 /* Confirm path validity */
#define MSG_RST       0x1000
#define MSG_ERRQUEUE  0x2000 /* Fetch message from error queue */
#define MSG_NOSIGNAL  0x4000 /* Do not generate SIGPIPE */
#define MSG_MORE      0x8000 /* Sender will send more */

#define MSG_EOF       MSG_FIN




/* Setsockoptions(2) level. Thanks to BSD these must match IPPROTO_xxx */
#define SOL_SOCKET 1
#define SOL_IP      0
//#define SOL_ICMP    1   /* No-no-no! Due to Linux :-) we cannot use SOL_ICMP=1 */
#define SOL_TCP     6
#define SOL_UDP     17
#define SOL_IPV6    41
//#define SOL_ICMPV6  58
//#define SOL_SCTP    132
//#define SOL_UDPLITE 136 /* UDP-Lite (RFC 3828) */
#define SOL_RAW     255
//#define SOL_IPX     256
//#define SOL_AX25    257
//#define SOL_ATALK   258
//#define SOL_NETROM  259
//#define SOL_ROSE    260
//#define SOL_DECNET  261
//#define SOL_X25     262
//#define SOL_PACKET  263
//#define SOL_ATM     264 /* ATM layer (cell level) */
//#define SOL_AAL     265 /* ATM Adaption Layer (packet level) */
//#define SOL_IRDA    266
//#define SOL_NETBEUI 267
//#define SOL_LLC     268
//#define SOL_DCCP    269
//#define SOL_NETLINK 270
//#define SOL_TIPC    271



/* specified by libc */
#define SHUT_RD   0 /* all receptions will be disallowed */
#define SHUT_WR   1 /* all transmissions will be disallowed */
#define SHUT_RDWR 2 /* all receptions and transmissions will be disallowed */

/* posix descriptions

int     accept(int socket, struct sockaddr *address,
             socklen_t *address_len);
int     bind(int socket, const struct sockaddr *address,
             socklen_t address_len);
int     connect(int socket, const struct sockaddr *address,
             socklen_t address_len);
int     getpeername(int socket, struct sockaddr *address,
             socklen_t *address_len);
int     getsockname(int socket, struct sockaddr *address,
             socklen_t *address_len);
int     getsockopt(int socket, int level, int option_name,
             void *option_value, socklen_t *option_len);
int     listen(int socket, int backlog);
ssize_t recv(int socket, void *buffer, size_t length, int flags);
ssize_t recvfrom(int socket, void *buffer, size_t length,
             int flags, struct sockaddr *address, socklen_t *address_len);
ssize_t recvmsg(int socket, struct msghdr *message, int flags);
ssize_t send(int socket, const void *message, size_t length, int flags);
ssize_t sendmsg(int socket, const struct msghdr *message, int flags);
ssize_t sendto(int socket, const void *message, size_t length, int flags,
             const struct sockaddr *dest_addr, socklen_t dest_len);
int     setsockopt(int socket, int level, int option_name,
             const void *option_value, socklen_t option_len);
int     shutdown(int socket, int how);
int     socket(int domain, int type, int protocol);
int     socketpair(int domain, int type, int protocol,
             int socket_vector[2]);
*/

__BEGIN_DECLS

/**
 * create an endpoint for communication.
 * @param protocol The protocol specifies a particular protocol to be used with the socket.
 * 	Normally only a single protocol exists to support a particular socket type within a
 * 	given protocol family, in which case protocol can be specified as 0.
 * @return On success, a file descriptor for the new socket is returned.
 * 	On error, result < 0.
 */
extern int socket(int domain, int type, int protocol);

/**
 * bind a socket to an address.
 * @param sockfd socket file descriptor
 * @param addr local address sockaddr structure
 * @param addrlen size of addr
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * setup of connection
 * @param sockfd socket description, was created by socket()
 * @param daddr destination address sockaddr structure
 * @param daddrlen size of daddr
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int connect(int sockfd, const struct sockaddr *daddr, socklen_t daddrlen);

/**
 * mark socket as accepting connections
 * @param sockfd socket descriptor
 * @param backlog limit of outstanding connections in sock queue
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int listen(int sockfd, int backlog);

/**
 * peek first connection request from queue and return fd of socket associated to
 * connected stream
 * @param sockfd socket description
 * @param addr NULL or pointer of sockaddr struct where address of connected socket
 * will be returened
 * @param addrlen length of addr struct
 * @return fd on success. -1 on failure with errno indicating error.
 */
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * send a message on a socket.
 * @param sockfd socket file descriptor
 * @param buf pointer to data
 * @param len length of buf
 * @param flags
 * @return the number of characters sent. -1 on failure with errno indicating error.
 */
extern ssize_t send(int sockfd, const void *buf, size_t len, int flags);

/**
 * send a message on a socket.
 * @param sockfd socket file descriptor
 * @param buf pointer to data
 * @param len length of buf
 * @param flags
 * @param daddr destination address sockaddr structure
 * @param daddrlen size of daddr
 * @return the number of characters sent. -1 on failure with errno indicating error.
 */
extern ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen);

/**
 * send a message on a socket.
 * @param sockfd socket file descriptor
 * @param msg
 * @param flags
 * @return the number of characters sent. -1 on failure with errno indicating error.
 */
extern ssize_t sendmsg(int socket, const struct msghdr *message,
		int flags);

/**
 * receive a message from a socket.
 * @param sockfd socket descriptor
 * @param buf buffer
 * @param len size of buffer
 * @param flags
 * @return the number of bytes received. -1 on failure with errno indicating error.
 */
extern ssize_t recv(int sockfd, void *buf, size_t len, int flags);

/**
 * receive a message from a socket.
 * @param sockfd socket descriptor
 * @param buf buffer
 * @param len size of buffer
 * @param flags
 * @param daddr destination address sockaddr structure
 * @param daddrlen size of daddr
 * @return the number of bytes received. -1 on failure with errno indicating error.
 */
extern ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
		struct sockaddr *daddr, socklen_t *daddrlen);
/**
 * receive a message from a socket.
 * @param sockfd socket descriptor
 * @param buf buffer
 * @param len size of buffer
 * @param flags
 * @return the number of bytes received. -1 on failure with errno indicating error.
 */
extern ssize_t recvmsg(int socket, struct msghdr *message,
		int flags);

/**
 * shut down part of a full-duplex connection
 * @param sockfd socket descriptor
 * @param how is one of SHUT_RD, SHUT_WR or SHUT_RDWR
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int shutdown(int sockfd, int how);

extern int getsockname(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen);

extern int getpeername(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen);

/**
 * get socket options
 * @param sockfd socket description
 * @param level option level (SOL_SOCKET or protocol number)
 * @param optname option name (like SO_BROADCAST or SO_LINGER)
 * @param optval pointer to option value variable on return
 * @param optlen pointer to length of the option value variable on return
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int getsockopt(int sockfd, int level, int optname,
		void *optval, socklen_t *optlen);

/**
 * set socket options
 * @param sockfd socket description
 * @param level option level (SOL_SOCKET or protocol number)
 * @param optname option name (like SO_BROADCAST or SO_LINGER)
 * @param optval option value to set
 * @param optlen length of the option value
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int setsockopt(int sockfd, int level, int optname,
		const void *optval, socklen_t optlen);
/**
 * The socketpair() call creates an unnamed pair of connected
 *      sockets in the specified domain, of the specified type, and using
 *      the optionally specified protocol.  For further details of these
 *      arguments, see socket(2).
 *      The file descriptors used in referencing the new sockets are
 *      returned in sv[0] and sv[1].  The two sockets are
 *      indistinguishable.
 */
extern int socketpair(int domain, int type, int protocol, int sv[2]);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_SOCKET_H_ */
