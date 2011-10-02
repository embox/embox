/**
 * @file
 * @brief Binary not-balanced tree data structure. There can be not more than one equal element in structure.
 *
 * @date Oct 1, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_H_
#define UTIL_TREE_H_

#include <assert.h>
#include <util/member.h>

struct tree_link {
	struct tree_link *left;
	struct tree_link *right;
};

/**
 * Compare two elements of tree.
 *
 * @param first
 * @param second
 * @return
 *   The result of comparison of the @a first and the @a second.
 */
typedef int (*tree_comparator_t)(struct tree_link *first, struct tree_link *second);

struct tree {
	struct tree_link *root;
};

#define tree_element(link, element_type, link_member) \
	member_cast_out(link, element_type, link_member)

#include __impl_x(util/tree_impl.h)

/**
 * Initialize tree.
 * @param tree Tree to initialize.
 */
extern struct tree *tree_init(struct tree *tree);

/**
 * Initialize tree link.
 * @param tree Link to initialize.
 */
extern struct tree_link *tree_link_init(struct tree_link *link);

/**
 * Check whether the tree is empty.
 * @param tree
 * @return true iff tree is empty.
 */
extern bool tree_empty(struct tree *tree);

/**
 * Find element in the tree.
 * Element is considered to be found if result of compare function is zero.
 * @param tree Tree, where element is searched
 * @param link Element, equal to which is found.
 * @param compare Compare function
 * @return
 * 	NULL, if there is no such element in tree.
 *	Pointer to this element otherwise.
 */
extern struct tree_link *tree_find_link(struct tree *tree,
		struct tree_link *link, tree_comparator_t compare);

/**
 * Add element to tree.
 * If there was an element equaled to added, it would be replaced.
 * @param tree Tree, element is inserted into
 * @param link Added element
 * @param compare Compare function
 * @return
 * 	true, if element wasn't in tree.
 */
extern bool tree_add_link(struct tree *tree,
		struct tree_link *link, tree_comparator_t compare);

/**
 * Delete element from tree, which compare result with given element is zero.
 * @param tree Tree, element is deleted from
 * @param link Deleted element
 * @param compare Compare function
 * @return
 * 	true, if element was in tree before deletion.
 */
extern bool tree_remove_link(struct tree* tree,
		struct tree_link *link, tree_comparator_t compare);

/**
 * Return the most left element of the tree.
 * According to tree structure, this element must be the smallest one.
 * @param tree
 * @return The smallest element of the tree.
 */
extern struct tree_link *tree_min_link(struct tree *tree);

/**
 * Return the most right element of the tree.
 * According to tree structure, this element must be the greatest one.
 * @param tree
 * @return The greatest element of the tree.
 */
extern struct tree_link *tree_max_link(struct tree *tree);

#endif /* UTIL_TREE_H_ */
