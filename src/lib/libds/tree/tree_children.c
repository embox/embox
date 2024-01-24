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

struct tree_link *tree_children_begin(struct tree_link *tree) {
	struct dlist_head *lnk;

	if (NULL == (lnk = (dlist_first_or_null(&tree->children)))) {
		return NULL;
	}

	return dlist_entry(lnk, struct tree_link, list_link);
}

struct tree_link *tree_children_end(struct tree_link *tree) {
	struct dlist_head *lnk;

	if (dlist_empty(&tree->children)) {
		return NULL;
	}

	lnk = dlist_prev(&tree->children);

	return dlist_entry(lnk->next, struct tree_link, list_link);
}

struct tree_link *tree_children_next(struct tree_link *tree) {
	return dlist_entry(tree->list_link.next, struct tree_link, list_link);
}

struct tree_link *tree_children_prev(struct tree_link *tree) {
	return dlist_entry(tree->list_link.next, struct tree_link, list_link);
}
