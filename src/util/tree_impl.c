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
#include <stddef.h>

struct tree_link {
	/* Parent node in the tree. */
	struct tree_link *par;

	/* Children are represented as a list of nodes. */
	struct list children;

	/* List link inside of list of children. */
	struct list_link list_link;
};

static inline struct tree_link *tree_link_init(struct tree_link *link) {
	assert(link != NULL);
	list_init(&link->children);
	list_link_init(&link->list_link);
	link->par = NULL;
	return link;
}

static inline void tree_add_link(struct tree_link *parent, struct tree_link *link) {
	assert(parent != NULL);
	assert(link != NULL);
	list_add_first_link(&link->list_link, &parent->children);
	link->par = parent;
}

static inline int tree_remove_link(struct tree_link *link) {
	assert(link != NULL);
	if (link->par != NULL) {
		list_unlink_link(&link->list_link);
		link->par = NULL;
		return 1;
	}
	return 0;
}

#endif /* UTIL_TREE_IMPL_C_ */
