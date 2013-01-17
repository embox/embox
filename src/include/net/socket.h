/**
 * @file
 *
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

#include <sys/socket.h>

/* maximum number of socket connections */
#define MAX_SYSTEM_CONNECTIONS 4

/* Supported address families. */
#define N_FAMILIES_SUPPORTED 4


/* Protocol families, same as address families. */
#define PF_UNSPEC	AF_UNSPEC
#define PF_UNIX		AF_UNIX
#define PF_LOCAL	AF_LOCAL
#define PF_INET		AF_INET
#define PF_PACKET	AF_PACKET
//#define PF_MAX		AF_MAX

#define PNET_GRAPH   40

/* Maximum queue length specifiable by listen. */
#define SOMAXCONN 4




/* IPX options */
#define IPX_TYPE    1



/**< Internet address in network representation */
struct in_addr {
	uint32_t    s_addr;
};

typedef uint32_t in_addr_t;

/* more specific address structure
	 (to recast on sockaddr_t in AF_INET sockets) */
typedef struct sockaddr_in {
	unsigned short   sin_family;   /* e.g. AF_INET */
	__be16		 sin_port;     /* e.g. htons(3490) i.e. port in big endian */
	struct in_addr   sin_addr;     /* see struct in_addr, above */
	char             sin_zero[8];  /* zero this if you want to */
} sockaddr_in_t;

struct sock;
struct sk_buff;
/**
 * Handle encapsulated protocol.
 * @return 0 on success.
 * @return -1 on failure and drop packet.
 */
typedef int (* sk_encap_hnd) (struct sock *sk, struct sk_buff *pack);



/**
 * close a socket descriptor
 * @param sockfd socket description
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int socket_close(int sockfd);


extern int check_icmp_err(int sockfd);

extern void socket_set_encap_recv(int sockfd, sk_encap_hnd hnd);

#endif /* NET_SOCKET_H_ */
