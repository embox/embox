/**
 * @file
 * @brief Implementation of methods in util/tree.h
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#include <assert.h>
#include <stddef.h>

#include <util/dlist.h>
#include <util/tree.h>

struct tree_link *tree_link_init(struct tree_link *link) {
	assert(link != NULL);
	dlist_init(&link->children);
	dlist_head_init(&link->list_link);
	link->par = NULL;
	return link;
}

void tree_add_link(struct tree_link *parent, struct tree_link *link) {
	assert(parent != NULL);
	assert(link != NULL);
	assert(link->par == NULL);
	dlist_add_prev(&link->list_link, &parent->children);
	link->par = parent;
}

void tree_move_link(struct tree_link *parent, struct tree_link *link) {
	assert(link != NULL);
	assert(parent != NULL);
	if (parent != link->par) {
		tree_unlink_link(link);
		tree_add_link(parent, link);
	}
}

int tree_unlink_link(struct tree_link *link) {
	assert(link != NULL);
	if (link->par != NULL) {
		dlist_del_init(&link->list_link);
		link->par = NULL;
		return 1;
	}
	return 0;
}

void tree_delete_link(struct tree_link *link,
    void dispose(struct tree_link *)) {
	struct tree_link *iter;
	assert(link != NULL);
	tree_unlink_link(link);
	tree_postorder_traversal_link_safe(iter, link) {
		tree_link_init(iter);
		dispose(iter);
	}
}
