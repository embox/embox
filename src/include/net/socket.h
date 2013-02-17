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

#include <netinet/in.h>

struct sock;
struct sk_buff;

/**
 * Handle encapsulated protocol.
 * @return 0 on success.
 * @return -1 on failure and drop packet.
 */
typedef int (* sk_encap_hnd) (struct sock *sk, struct sk_buff *pack);

#if 0
/**
 * close a socket descriptor
 * @param sockfd socket description
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int socket_close(int sockfd);
#endif

extern int check_icmp_err(int sockfd);

extern void socket_set_encap_recv(int sockfd, sk_encap_hnd hnd);

#endif /* NET_SOCKET_H_ */
