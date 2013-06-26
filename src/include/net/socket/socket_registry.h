/**
 * @file
 * @brief Socket registry maintenance header
 *
 * @date 23.03.12
 * @author Timur Abdukadyrov
 */

#ifndef NET_SOCKET_SOCKET_REGISTRY_H_
#define NET_SOCKET_SOCKET_REGISTRY_H_

#include <net/socket/socket.h>
#include <util/dlist.h>

enum socket_connection_state_t {UNCONNECTED,
 CLOSED, LISTENING, BOUND, CONNECTING, CONNECTED, ESTABLISHED, DISCONNECTING, SOCK_CONN_STATE_MAX};

/**
 * @param sock socket connected to addr
 * @param addr address connected to sock
 */
typedef struct socket_node{
	struct dlist_head link;
	struct socket *sock;
	struct sockaddr saddr;
	struct sockaddr daddr;
	enum socket_connection_state_t socket_connection_state;
} socket_node_t;

/* TODO: add descriptions */

/* inner functions for address binding maintance */
extern int sr_add_socket_to_registry(struct socket *sock);
extern int sr_remove_socket_from_registry(struct socket *sock);
extern int sr_set_saddr(struct socket *sock, const struct sockaddr *addr);
extern void sr_remove_saddr(struct socket *sock);
extern bool sr_socket_exists(struct socket *sock);

extern bool sr_is_saddr_free(struct socket *sock, struct sockaddr *addr);
extern bool sr_is_daddr_free(struct socket *sock, struct sockaddr *addr);

/**
 * @brief This structure is used to provide access to the opened sockets from common applications
 * @see  sr_get_all_sockets_array, sr_free_all_sockets_array, sr_get_all_sockets_count
 */
struct sr_external_socket_array_node{
	struct sockaddr saddr;
	struct sockaddr daddr;
	enum socket_connection_state_t socket_connection_state;
};
extern struct sr_external_socket_array_node *sr_get_all_sockets_array(size_t *length);
extern void sr_free_all_sockets_array(struct sr_external_socket_array_node *array);
extern size_t sr_get_all_sockets_count(void);

/* socket information node connection info methods. could be excess */
static inline void sk_set_connection_state(struct socket *sock, enum socket_connection_state_t state){
	sock->socket_node->socket_connection_state = state;
}

static inline enum socket_connection_state_t sk_get_connection_state(struct socket *sock){
	return sock->socket_node->socket_connection_state;
}

static inline int sk_is_connected(struct socket *sock){
	return (sock->socket_node->socket_connection_state == CONNECTED
			|| sock->socket_node->socket_connection_state == ESTABLISHED);
}

static inline int sk_is_bound(struct socket *sock){
	return (sock->socket_node->socket_connection_state == BOUND);
}

static inline int sk_is_listening(struct socket *sock){
	return (sock->socket_node->socket_connection_state == LISTENING);
}

/**
 * set pending error in socket registry node for struct socket entity
 **/
static inline void so_sk_set_so_error(struct sock *sk, int error){
	if (sk){
		assert(sk->sk_socket != NULL);
		sk->opt.so_error = error;
	}
	else
		return;
}

#endif /* NET_SOCKET_SOCKET_REGISTRY_H_ */
