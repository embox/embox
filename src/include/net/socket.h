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
#include <netinet/in.h>

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
