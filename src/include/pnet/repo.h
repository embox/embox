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
	/*connect;
	start;
	stop;
	pause;
	disconnect;
	*/
};

#define PNET_MODULE(str_id,pnode) \
	extern const struct pnet_module __pnet_mod_repo[]; \
	ARRAY_SPREAD_ADD(__pnet_mod_repo, { \
		.name = str_id, \
		.node = &pnode \
	})


extern struct net_node *pnet_get_module(const char *name);

#endif /* PNET_REPO_H_ */
