/**
 * @file
 * @brief Implementation of methods in util/tree.h
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#include <assert.h>
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

void tree_delete_link(struct tree_link *link, void dispose(struct tree_link *)) {
	struct tree_link *iter;
	assert(link != NULL);
	tree_unlink_link(link);
	tree_postorder_traversal_link_safe(iter, link) {
		tree_link_init(iter);
		dispose(iter);
	}
}

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
	} else {
		/* Next link is a parent */
		return link->par;
	}
}

struct tree_link *tree_postorder_begin(struct tree_link *tree) {
	if (tree == NULL) {
		return NULL;
	}
	while (!dlist_empty(&tree->children)) {
		tree = dlist_entry(dlist_first(&tree->children),
					struct tree_link, list_link);
	}
	return tree;
}

struct tree_link *tree_postorder_end(struct tree_link *tree) {
	return tree_postorder_next(tree);
}

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

struct tree_link *tree_children_begin(struct tree_link *tree) {
	struct dlist_head *lnk;

	if (NULL == (lnk = (dlist_first_or_null(&tree->children)))) {
		return NULL;
	}

	return dlist_entry(lnk, struct tree_link, list_link);
}

struct tree_link *tree_children_end(struct tree_link *tree) {
	struct dlist_head *lnk;

	if (NULL == (lnk = (!dlist_empty(&tree->children) ? dlist_prev(&tree->children) : NULL))) {
		return NULL;
	}

	return dlist_entry(lnk->next, struct tree_link, list_link);
}


struct tree_link *tree_children_next(struct tree_link *tree) {
	    return dlist_entry(tree->list_link.next, struct tree_link, list_link);
}

struct tree_link *tree_children_prev(struct tree_link *tree) {
	    return dlist_entry(tree->list_link.next, struct tree_link, list_link);
}
