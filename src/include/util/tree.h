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

typedef struct tree_link* pnode;
struct tree_link {
	pnode left;
	pnode right;
};

/**
 * Compare two elements of tree.
 *
 * @param first
 * @param second
 * @return
 *   The result of comparison of the @a first and the @a second.
 */
typedef int (*tree_comparator_t)(pnode first, pnode second);

struct tree {
	pnode root;
	tree_comparator_t comparator;
};

#define tree_element(link, element_type, link_member) \
	member_cast_out(link, element_type, link_member)


inline struct tree *tree_init(struct tree *tree, tree_comparator_t comparator) {
	assert(tree != NULL);
	assert(comparator != NULL);
	tree->root = NULL;
	tree->comparator = comparator;
	return tree;
}

inline pnode tree_link_init(pnode link) {
	assert(link != NULL);
	link->left = link->right = NULL;
	return link;
}

#include __impl_x(util/tree_impl.h)

inline int tree_empty(struct tree *tree) {
	assert(tree != NULL);
	return tree->root == NULL;
}

/**
 * Find element in the tree.
 * Element is considered to be found if result of compare function is zero.
 * @param tree Tree, where element is searched
 * @param link Element, equal to which is found.
 * @return
 * 	NULL, if there is no such element in tree.
 *	Pointer to this element otherwise.
 */
inline pnode tree_find_link(struct tree* tree, pnode link);

/**
 * Add element to tree.
 * If there was an element equaled to added, it would be replaced.
 * @param tree Tree, element is inserted into
 * @param link Added element
 * @return
 * 	true, if element wasn't in tree.
 */
bool tree_add_link(struct tree* tree, pnode link);

/**
 * Delete element from tree, which compare result with given element is zero.
 * @param tree Tree, element is deleted from
 * @param link Deleted element
 * @return
 * 	true, if element was in tree before deletion.
 */
bool tree_remove_link(struct tree* tree, pnode link);

/**
 * Return the most left element of the tree.
 * According to tree structure, this element must be the smallest one.
 * @param tree
 * @return The smallest element of the tree.
 */
inline pnode tree_min_link(struct tree *tree);

/**
 * Return the most right element of the tree.
 * According to tree structure, this element must be the greatest one.
 * @param tree
 * @return The greatest element of the tree.
 */
inline pnode tree_max_link(struct tree *tree);


#endif /* UTIL_TREE_H_ */
