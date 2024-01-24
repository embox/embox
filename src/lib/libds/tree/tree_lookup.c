/**
 * @file
 * @brief Implementation of methods in util/tree.h
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#include <assert.h>
#include <stddef.h>

#include <lib/libds/dlist.h>
#include <lib/libds/tree.h>

struct tree_link *tree_lookup_child(struct tree_link *node,
    tree_predicate_t predicate, void *arg) {
	struct tree_link *link;

	assert(node && predicate);

	dlist_foreach_entry(link, &node->children, list_link) {
		if (predicate(link, arg)) {
			return link;
		}
	}

	return NULL;
}

struct tree_link *tree_lookup(struct tree_link *node,
    tree_predicate_t predicate, void *arg) {
	struct tree_link *link;

	assert(node && predicate);

	tree_postorder_traversal_link(link, node) {
		if (predicate(link, arg)) {
			return link;
		}
	}

	return NULL;
}
