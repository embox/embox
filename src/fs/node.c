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

#include <lib/list.h>
#include <linux/init.h>

#include <fs/node.h>

typedef struct node_head {
	struct list_head *prev;
	struct list_head *next;
	node_t            node;
} node_head_t;

static node_head_t node_pool[OPTION_GET(NUMBER,fnode_quantity)];
static LIST_HEAD(head_node);

#define node_to_head(node_) (uint32_t)(node_ - offsetof(node_head_t, node))

EMBOX_UNIT_INIT(node_init);

static int __init node_init(void) {
	size_t i;

	for (i = 0; i < ARRAY_SIZE(node_pool); i++) {
		list_add((struct list_head *) &node_pool[i], &head_node);
	}
	return 0;
}

node_t *alloc_node(const char *name) {
	node_head_t *head;
	node_t *node;

	if (list_empty(&head_node)) {
		return NULL;
	}
	head = (node_head_t *) (&head_node)->next;
	list_del((&head_node)->next);
	node = &(head->node);
	strcpy((char*) node->name, name);
	tree_link_init(&node->tree_link);
	return node;
}

void free_node(node_t *node) {
	if (NULL == node) {
		return;
	}
	list_add((struct list_head *) node_to_head(node),
		    (struct list_head *)&head_node);
}
