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

POOL_DEF(node_pool, struct node, OPTION_GET(NUMBER,fnode_quantity));

EMBOX_UNIT_INIT(node_init);

static int node_init(void) {
	return 0;
}

node_t *alloc_node(const char *name) {
	node_t *node;

	node = pool_alloc(&node_pool);
	if(NULL == node) {
		return NULL;
	}

	strcpy((char*) node->name, name);
	tree_link_init(&node->tree_link);
	return node;
}

void free_node(node_t *node) {
	if (NULL == node) {
		return;
	}
	pool_free(&node_pool, node);
}
