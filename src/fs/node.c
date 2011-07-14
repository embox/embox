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
	node_t            nod;
} node_head_t;

static node_head_t node_pool[CONFIG_QUANTITY_NODE];
static LIST_HEAD(head_node);

#define nod_to_head(node) (uint32_t)(node - offsetof(node_head_t, nod))

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
	node_t *nod;

	if (list_empty(&head_node)) {
		return NULL;
	}
	head = (node_head_t *) (&head_node)->next;
	list_del((&head_node)->next);
	nod = &(head->nod);
	strcpy((char*) nod->name, name);
	INIT_LIST_HEAD(&nod->leaves);
	INIT_LIST_HEAD(&nod->neighbors);
	return nod;
}

void free_node(node_t *node) {
	if (NULL == node) {
		return;
	}
	list_add((struct list_head *) nod_to_head(node),
		    (struct list_head *)&head_node);
}
