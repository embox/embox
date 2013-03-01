/**
 * @file
 * @brief pnet proto definitions
 *
 * @date 14.11.2011
 * @author Anton Kozlov
 */

#ifndef PNET_PROTO_H_
#define PNET_PROTO_H_

#include <stddef.h>

struct net_node;

struct pnet_pack;

typedef int (*net_hnd)(struct pnet_pack *pack);
typedef int (*net_node_hnd)(struct net_node *node);
typedef struct net_node *(*net_alloc_hnd)(void);

struct pnet_node_actions {
	net_hnd rx_hnd;
	net_hnd tx_hnd;
	net_alloc_hnd alloc;
	net_node_hnd free;
	net_node_hnd start;
	net_node_hnd stop;
};

typedef struct pnet_proto {
	const char *name;
	struct pnet_node_actions actions;
} *pnet_proto_t;

#if 0
#define __ACCESSOR(fun_name, res_type, fun) \
	static inline res_type fun_name(struct net_node *node) { \
		return node->proto->actions.fun;\
	}

#define __ACCESSOR_PREF(pref, res_type, fun) \
	__ACCESSOR(pref##fun, res_type, fun)


__ACCESSOR_PREF(pnet_proto_, net_hnd, rx_hnd)
__ACCESSOR_PREF(pnet_proto_, net_hnd, tx_hnd)
__ACCESSOR_PREF(pnet_proto_, net_node_hnd, start)
__ACCESSOR_PREF(pnet_proto_, net_node_hnd, stop)
__ACCESSOR_PREF(pnet_proto_, net_alloc_hnd, alloc)
__ACCESSOR_PREF(pnet_proto_, net_node_hnd, free)
#endif

#define __ACCESSOR(pref, res_type, fun) \
	static inline res_type pref##fun(struct net_node *node) { \
		return (node->proto ? node->proto->actions.fun : NULL);\
	}

__ACCESSOR(pnet_proto_, net_hnd, rx_hnd)
__ACCESSOR(pnet_proto_, net_hnd, tx_hnd)
__ACCESSOR(pnet_proto_, net_node_hnd, start)
__ACCESSOR(pnet_proto_, net_node_hnd, stop)
__ACCESSOR(pnet_proto_, net_alloc_hnd, alloc)
__ACCESSOR(pnet_proto_, net_node_hnd, free)

#endif
