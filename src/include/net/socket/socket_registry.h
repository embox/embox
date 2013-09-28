/**
 * @file
 * @brief Socket registry maintenance header
 *
 * @date 23.03.12
 * @author Timur Abdukadyrov
 */

#ifndef NET_SOCKET_SOCKET_REGISTRY_H_
#define NET_SOCKET_SOCKET_REGISTRY_H_

#include <util/dlist.h>
#include <net/sock.h>


/**
 * @param sock socket connected to addr
 * @param addr address connected to sock
 */
typedef struct socket_node{
	struct dlist_head link;
	struct sock *sk;
	struct sockaddr saddr;
	struct sockaddr daddr;
} socket_node_t;

/* TODO: add descriptions */

/* inner functions for address binding maintance */
extern int sr_add_socket_to_registry(struct sock *sk);
extern int sr_remove_socket_from_registry(struct sock *sk);
extern int sr_set_saddr(struct sock *sk, const struct sockaddr *addr);
extern void sr_remove_saddr(struct sock *sk);
extern bool sr_socket_exists(struct sock *sk);

extern bool sr_is_saddr_free(struct sock *sk, struct sockaddr *addr);
extern bool sr_is_daddr_free(struct sock *sk, struct sockaddr *addr);

/**
 * @brief This structure is used to provide access to the opened sockets from common applications
 * @see  sr_get_all_sockets_array, sr_free_all_sockets_array, sr_get_all_sockets_count
 */
struct sr_external_socket_array_node{
	struct sockaddr saddr;
	struct sockaddr daddr;
	enum sock_state socket_connection_state;
};
extern struct sr_external_socket_array_node *sr_get_all_sockets_array(size_t *length);
extern void sr_free_all_sockets_array(struct sr_external_socket_array_node *array);
extern size_t sr_get_all_sockets_count(void);

/* socket information node connection info methods. could be excess */

/**
 * set pending error in socket registry node for struct socket entity
 **/
static inline void so_sk_set_so_error(struct sock *sk, int error){
	if (sk) {
		sk->opt.so_error = error;
	}
}

#endif /* NET_SOCKET_SOCKET_REGISTRY_H_ */
