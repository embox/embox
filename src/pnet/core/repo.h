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

#include <lib/libds/array.h>
#include <util/macro.h>
#include <pnet/core/types.h>

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
	ARRAY_SPREAD_DECLARE(const struct pnet_module, __pnet_mod_repo); \
	ARRAY_SPREAD_ADD(__pnet_mod_repo, { \
		.name = str_id,             \
		.node = &pnode,             \
		.proto = NULL               \
	})

#define __PNET_REPO_PROTO_ADD(str_id, pproto) \
	ARRAY_SPREAD_DECLARE(const struct pnet_module, __pnet_mod_repo); \
	ARRAY_SPREAD_ADD(__pnet_mod_repo, { \
		.name = str_id,             \
		.node = NULL,               \
		.proto = &pproto            \
	})


#define __PNET_PROTO_DEF_NAME(str_id, _name, ...) \
	static struct pnet_proto _name = { \
		.name = str_id,            \
		.actions = __VA_ARGS__     \
	}

#define __PNET_NODE_DEF_NAME(str_id, node_name, proto_name) \
	static struct net_node node_name = { \
		.name  = str_id,                 \
		.proto = &proto_name             \
	}

#define __PNET_PROTO_DEF_NAME_REPO(str_id, name, ...) \
	__PNET_PROTO_DEF_NAME(str_id, name, __VA_ARGS__); \
	__PNET_REPO_PROTO_ADD(str_id, name)

#define __PNET_NODE_DEF_NAME_REPO(str_id, name, proto_name, ...) \
	__PNET_PROTO_DEF_NAME(str_id, proto_name, __VA_ARGS__); \
	__PNET_NODE_DEF_NAME(str_id, name, proto_name); \
	__PNET_REPO_NODE_ADD(str_id, name)


#define PNET_PROTO_DEF(str_id, ...) \
	__PNET_PROTO_DEF_NAME_REPO(str_id, MACRO_GUARD(proto), __VA_ARGS__)

#define PNET_NODE_DEF(str_id, ...)\
	__PNET_NODE_DEF_NAME_REPO(str_id, MACRO_GUARD(node), MACRO_GUARD(proto) , __VA_ARGS__ )

#define PNET_NODE_DEF_NAME(str_id, node_nm, ...)\
	__PNET_NODE_DEF_NAME_REPO(str_id, node_nm, MACRO_GUARD(proto), __VA_ARGS__ )

extern struct net_node *pnet_get_module(const char *name);

#ifdef __CDT_PARSER__

#define PNET_PROTO_DEF(str_id, ...) \
	__PNET_PROTO_DEF_NAME_REPO(str_id, MACRO_GUARD(proto), __VA_ARGS__); \
	typedef typeof(MACRO_GUARD(proto)) __pnet_proto_placeholder

#define PNET_NODE_DEF(str_id, ...)\
	PNET_NODE_DEF_NAME(str_id, MACRO_GUARD(node), __VA_ARGS__)

#define PNET_NODE_DEF_NAME(str_id, node_nm, ...)\
	__PNET_NODE_DEF_NAME_REPO(str_id, node_nm, MACRO_GUARD(proto), __VA_ARGS__ ); \
	typedef typeof(node_nm) __pnet_proto_placeholder; \
	typedef typeof(MACRO_GUARD(proto)) __pnet_proto_placeholder

#endif /* __CDT_PARSER__ */

#endif /* PNET_REPO_H_ */
