/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 */

#include <linux/init.h>
#include <embox/unit.h>
#include <embox/kernel.h>
#include <lib/list.h>
#include <string.h>
#include <fs/node.h>

static node_t node_pool[CONFIG_QUANTITY_NODE];
static LIST_HEAD(head_node);

EMBOX_UNIT_INIT(node_init);

static int __init node_init(void) {
	size_t i;

	for (i = 0; i < ARRAY_SIZE(node_pool); i++) {
		list_add((struct list_head *) &node_pool[i], &head_node);
	}
	return 0;
}

node_t *alloc_node(const char *name) {
	node_t *nod;

	if (list_empty(&head_node)) {
		return NULL;
	}
	nod = (node_t *) (&head_node)->next;
	list_del((&head_node)->next);
	strcpy((char*)nod->name, name);
	return nod;
}

void free_node(node_t *node) {
	if (NULL == node) {
		return;
	}
	list_add((struct list_head *) node,
		    (struct list_head *)&head_node);
	node = NULL;
}
