/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 */


#include <string.h>
#include <embox/unit.h>
#include <util/array.h>

#include <fs/node.h>

#include <mem/misc/pool.h>

#define MAX_NODE_QUANTITY OPTION_GET(NUMBER,fnode_quantity)
POOL_DEF(node_pool, struct node, MAX_NODE_QUANTITY);
POOL_DEF(nas_pool, struct nas, MAX_NODE_QUANTITY);
POOL_DEF(nodefinfo_pool, struct node_fi, MAX_NODE_QUANTITY);

EMBOX_UNIT_INIT(node_init);

static int node_init(void) {
	return 0;
}

node_t *node_alloc(const char *name) {
	node_t *node;
	struct nas *nas;

	node = pool_alloc(&node_pool);
	if(NULL == node) {
		return NULL;
	}

	nas = pool_alloc(&nas_pool);
	if(NULL == nas) {
		node_free(node);
		return NULL;
	}

	nas->fi = pool_alloc(&nodefinfo_pool);
	if(NULL == nas) {
		node_free(node);
		return NULL;
	}

	node->nas = nas;
	nas->node = node;

	strcpy((char*) node->name, name);
	tree_link_init(&node->tree_link);
	return node;
}

void node_free(node_t *node) {
	struct nas *nas;
	if (NULL == node) {
		return;
	}
	nas = node->nas;
	if (NULL == nas->fi) {
		pool_free(&nodefinfo_pool, nas->fi);
	}

	if (NULL == nas) {
		pool_free(&nas_pool, nas);
	}

	pool_free(&node_pool, node);
}
