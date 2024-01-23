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

struct tree_link *tree_postorder_next(struct tree_link *link) {
	if (link == NULL) {
		return NULL;
	}
	/* This node is a root. */
	if (link->par == NULL) {
		return NULL;
	}
	/* Search for the next element in brother list. */
	if (&link->list_link != (dlist_last_or_null(&link->par->children))) {
		/* It's not a last element in list. */
		/* TODO dirty hack. XXX O(n) time. */
		return tree_postorder_begin(
		    dlist_entry(link->list_link.next, struct tree_link, list_link));
	}
	else {
		/* Next link is a parent */
		return link->par;
	}
}

struct tree_link *tree_postorder_begin(struct tree_link *tree) {
	if (tree == NULL) {
		return NULL;
	}
	while (!dlist_empty(&tree->children)) {
		tree = dlist_entry(dlist_first(&tree->children), struct tree_link,
		    list_link);
	}
	return tree;
}

struct tree_link *tree_postorder_end(struct tree_link *tree) {
	return tree_postorder_next(tree);
}
