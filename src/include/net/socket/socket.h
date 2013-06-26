/**
 * @file
 * @brief
 *
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_SOCKET_SOCKET_H_
#define NET_SOCKET_SOCKET_H_



struct sock;
struct idx_desc;
struct socket_node;

/**
 * General BSD socket
 * @param state socket state (%SS_CONNECTED, etc)
 * @param type socket type (%SOCK_STREAM, etc)
 * @param flags socket flags (%SOCK_ASYNC_NOSPACE, etc)
 * @param sk internal networking protocol agnostic socket representation
 * @param desc idx descriptor of a socket. Note: in BSD sockets this field has type 'struct file'
 * @param socket_node node in global socket registry
 * @param ops protocol specific socket operations
 */
struct socket {
	struct sock *sk;
	struct idx_desc_data *desc_data;
	struct socket_node *socket_node;
};

#if 1 /********** TODO remove this ****************/

struct sk_buff;
/**
 * Handle encapsulated protocol.
 * @return 0 on success.
 * @return -1 on failure and drop packet.
 */
typedef int (*sk_encap_hnd)(struct sock *sk, struct sk_buff *pack);

extern void socket_set_encap_recv(int sockfd, sk_encap_hnd hnd);
#endif

#endif /* NET_SOCKET_SOCKET_H_ */
