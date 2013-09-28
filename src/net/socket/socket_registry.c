/**
 * @file
 * @brief Socket registry maintenance
 *
 * @date 23.03.12
 * @author Timur Abdukadyrov
 */

#include <errno.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <net/sock.h>
#include <net/socket/socket_registry.h>
#include <stdlib.h>
#include <string.h>
#include <util/dlist.h>
#include <util/sys_log.h>

/* maximum number of socket connections */
#define MODOPS_AMOUNT_SOCKET_NODE OPTION_GET(NUMBER, amount_socket_node)

/* system socket registry */
POOL_DEF(socket_node_pool, socket_node_t, MODOPS_AMOUNT_SOCKET_NODE);
DLIST_DEFINE(socket_registry);

static inline socket_node_t *get_sock_node_by_socket(struct sock *sk);
static socket_node_t *get_sock_node_by_src_address(struct sock *sk,
    struct sockaddr *addr);
static socket_node_t *get_sock_node_by_dst_address(struct sock *sk,
    struct sockaddr *addr);

int sr_add_socket_to_registry(struct sock *sk){
	socket_node_t *newnode;

	/* allocate new node */
	newnode = (socket_node_t *)pool_alloc(&socket_node_pool);
	newnode->link.list_id = 0;  /* 0_o */
	if (newnode == NULL)
		return -ENOMEM;

	/* set source address data to NULL for now*/
	memset(&newnode->saddr, 0, sizeof(struct sockaddr));
	/* set destination address data to NULL for now*/
	memset(&newnode->daddr, 0, sizeof(struct sockaddr));

	/* set socket link */
	newnode->sk = sk;

	sk->sock_node = newnode;

	/* list_add_tail(&socket_registry_head, &newnode->link); */
	dlist_add_next(&newnode->link, &socket_registry);

	LOG_INFO("add_socket_to_pool", "adding socket to pool");

	return ENOERR;
}

int sr_remove_socket_from_registry(struct sock *sk){
	socket_node_t *node;

	node = get_sock_node_by_socket(sk);
	if (node) {
		LOG_INFO("remove_socket_from_pool", "removing socket entity...");
		dlist_del(&node->link);
		pool_free(&socket_node_pool, node);
		return ENOERR;
	}
	return -1;
}

bool sr_is_saddr_free(struct sock *sk, struct sockaddr *addr){

	if (get_sock_node_by_src_address(sk, addr))
		return false;
	else
		return true;
}

bool sr_is_daddr_free(struct sock *sk, struct sockaddr *addr){

	if (get_sock_node_by_dst_address(sk, addr))
		return false;
	else
		return true;
}

bool sr_socket_exists(struct sock *sk){
	socket_node_t *node, *tmp;

	if (sk) {  /* address validity */
		dlist_foreach_entry(node, tmp, &socket_registry, link){
			if (sk == node->sk)
				return true;
		}
	}
	return false;
}

int sr_set_saddr(struct sock *sk, const struct sockaddr *addr){
	/* copy address data */
	memcpy(&sk->sock_node->saddr, addr, sizeof(struct sockaddr));

	LOG_INFO("bind_address", "bound address to socket");
	return 0;
}

void sr_remove_saddr(struct sock *sk){
	socket_node_t *node;

	node = get_sock_node_by_socket(sk);
	if (node) {
		LOG_INFO("unbind_socket", "found socket. trying to unbind...");
		memset(&node->saddr, 0, sizeof(struct sockaddr));
	}
}

static socket_node_t *get_sock_node_by_src_address(struct sock *sk, struct sockaddr *addr) {
	socket_node_t *node, *tmp;

	if (addr) {  /* address validity */
		dlist_foreach_entry(node, tmp, &socket_registry, link) {
			if (sk->f_ops->compare_addresses(addr, &node->saddr) &&
				 (sk->opt.so_type == node->sk->opt.so_type))
				return node;
		}
	}
	return NULL;
}

static socket_node_t *get_sock_node_by_dst_address(struct sock *sk, struct sockaddr *addr) {
	socket_node_t *node, *tmp;

	if (addr) {  /* address validity */
		dlist_foreach_entry(node, tmp, &socket_registry, link) {
			if (sk->f_ops->compare_addresses(addr, &node->daddr))
				return node;
		}
	}
	return NULL;
}

static inline socket_node_t *get_sock_node_by_socket(struct sock *sk) {
	return sk ? sk->sock_node : NULL;
}

size_t sr_get_all_sockets_count(void) {
	size_t i = 0;
	socket_node_t *node, *tmp;
	dlist_foreach_entry(node, tmp, &socket_registry, link) {
		i++;
	}
	return i;
}

struct sr_external_socket_array_node *sr_get_all_sockets_array(size_t *length) {
	size_t count, i = 0;
	socket_node_t *node, *tmp;
	struct sr_external_socket_array_node *array;

	count = sr_get_all_sockets_count();
	*length = count;
	array = malloc(sizeof(struct sr_external_socket_array_node) * (count + 1));
	if (!array)
		return NULL;

	dlist_foreach_entry(node, tmp, &socket_registry, link) {
		if (i == count)
			break;
		memcpy(&(array[i].saddr), &(node->saddr), sizeof(struct sockaddr));
		memcpy(&(array[i].daddr), &(node->daddr), sizeof(struct sockaddr));
		array[i].socket_connection_state = node->sk->state;
		i++;
	}
	return array;
}

void sr_free_all_sockets_array(struct sr_external_socket_array_node *array) {
	if (array)
		free(array);
}
