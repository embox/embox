/**
 * @file
 * @brief Implementation of methods in util/tree.h
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_IMPL_C_
#define UTIL_TREE_IMPL_C_

#include <assert.h>
#include <util/list.h>
#include <util/tree.h>

inline struct tree_link *tree_link_init(struct tree_link *link) {
	assert(link != NULL);
	list_init(&link->children);
	list_link_init(&link->list_link);
	link->par = NULL;
	return link;
}

inline void tree_add_link(struct tree_link *parent, struct tree_link *link) {
	assert(parent != NULL);
	assert(link != NULL);
	list_add_first_link(&link->list_link, &parent->children);
	link->par = parent;
}

inline int tree_remove_link(struct tree_link *link) {
	assert(link != NULL);
	if (link->par != NULL) {
		list_unlink_link(&link->list_link);
		link->par = NULL;
		return 1;
	}
	return 0;
}

inline struct tree_link *tree_next_link(struct tree_link *link) {
	assert(link != NULL);
	/* This node is a root. */
	if (link->par == NULL) {
		return NULL;
	}
	/* Search for the next element in brother list. */
	if (&link->list_link != list_last_link(&link->par->children)) {
		/* It's not a last element in list. */
		/* TODO dirty hack. */
		return list_element(link->list_link.next, struct tree_link, list_link);
	} else {
		/* Next link is a parent */
		return link->par;
	}
}

inline struct tree_link *tree_begin(struct tree_link *tree) {
	if (tree == NULL) {
		return NULL;
	}
	while (!list_is_empty(&tree->children)) {
		tree = list_element(list_first_link(&tree->children)
				, struct tree_link, list_link);
	}
	return tree;
}

#endif /* UTIL_TREE_IMPL_C_ */
