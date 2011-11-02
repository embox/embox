/**
 * @file
 *
 * @brief
 *
 * @date 01.11.2011
 * @author Anton Bondarev
 */

#ifndef PNET_REPO_H_
#define PNET_REPO_H_

#include <util/array.h>

struct pnet_module {
	const char *name;
	int type;
	struct net_node *node;
	struct pnet_proto *proto;
	/*connect;
	start;
	stop;
	pause;
	disconnect;
	*/
};

#define __PNET_REPO_NODE_ADD(str_id,pnode) \
	extern const struct pnet_module __pnet_mod_repo[]; \
	ARRAY_SPREAD_ADD(__pnet_mod_repo, { \
		.name = str_id, \
		.node = &pnode, \
		.proto = NULL\
	})

#define __PNET_REPO_PROTO_ADD(str_id, pproto) \
	extern const struct pnet_module __pnet_mod_repo[]; \
	ARRAY_SPREAD_ADD(__pnet_mod_repo, { \
		.name = str_id, \
		.node = NULL,  \
		.proto = &pproto\
	})


#define __PNET_PROTO_DEF_NAME(name, rx, tx, fr) \
	static struct pnet_proto name = {\
		.rx_hnd = rx, \
		.tx_hnd = tx, \
		.free   = fr \
	};

#define __PNET_NODE_DEF_NAME(name, proto_name) \
	static struct net_node name = {\
		.proto = &proto_name\
	};

#define __PNET_PROTO_DEF_NAME_REPO(str_id, name, rx, tx, fr) \
	__PNET_PROTO_DEF_NAME(name, rx, tx, fr) \
	__PNET_REPO_PROTO_ADD(str_id, name)

#define __PNET_NODE_DEF_NAME_REPO(str_id, name, proto_name, rx, tx) \
	__PNET_PROTO_DEF_NAME(proto_name, rx, tx, NULL) \
	__PNET_NODE_DEF_NAME(name, proto_name) \
	__PNET_REPO_NODE_ADD(str_id, name)


#define PNET_PROTO_DEF(str_id, rx, tx, fr) \
	__PNET_PROTO_DEF_NAME_REPO(str_id, __##rx##__##tx##__##fr##__proto, rx, tx, fr)

#define PNET_NODE_DEF(str_id, rx, tx)\
	__PNET_NODE_DEF_NAME_REPO(str_id, __##rx##__##tx##__node, __##rx##__##tx##__proto, rx, tx)

extern struct net_node *pnet_get_module(const char *name);

#endif /* PNET_REPO_H_ */
