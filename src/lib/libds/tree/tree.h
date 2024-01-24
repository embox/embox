/**
 * @file
 * @brief Tree data structure.
 *
 * @date 08.10.11
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_H_
#define UTIL_TREE_H_

#include <util/member.h>
#include <util/macro.h>
#include <util/dlist.h>

#define tree_element(link, element_type, link_member) \
	(link == NULL ? NULL \
		 : member_cast_out(link, element_type, link_member))

/**
 * Link on element of tree, keeping in each element.
 */
struct tree_link {
	/** Parent node in the tree. */
	struct tree_link *par;

	/** Children are represented as a list of nodes. */
	struct dlist_head children;

	/** List link inside of list of children. */
	struct dlist_head list_link;
};

/**
 * Type of functions, returning true or false for given tree_link.
 */
typedef int (*tree_predicate_t)(struct tree_link *link, void *arg);

/**
 * Initializes tree link.
 *
 * @param link Link to initialize.
 * @return The argument.
 */
extern struct tree_link *tree_link_init(struct tree_link *link);

/**
 * Add element to tree by adding new link into list of children of another node.
 *   Added tree_link must be initialized before and have no parent.
 *   Added element will be insert in the end of list of children.
 * @param parent Parent of new node.
 * @param link Added element
 */
extern void tree_add_link(struct tree_link *parent, struct tree_link *link);

/**
 * Unlink specified tree_link from its parent and bind it to new parent.
 * @param parent Parent of new node.
 * @param link Added element
 */
extern void tree_move_link(struct tree_link *parent, struct tree_link *link);

/**
 * Separates node from its parent.
 *   This parent can not exist.
 * @param link Unlinked element
 * @return
 * 	true, if element was in the tree before deletion.
 */
extern int tree_unlink_link(struct tree_link *link);

/**
 * Delete all subtree with this node.
 * For each element dispose method will be called,
 * @param link Link, what subtree will be deleted.
 * @param dispose Dispose method for each element.
 */
extern void tree_delete_link(struct tree_link *link, void dispose(struct tree_link *));

/**
 * 'Next' link in the tree according to selected order. Used for iteration.
 * The order is: firstly we recursively enumerate all children of node,
 *   then the node itself.
 * @param link Current node.
 * @return Next node in the tree.
 */
extern struct tree_link *tree_postorder_next(struct tree_link *link);

/**
 * First node for enumeration.
 * @param tree Tree to enumerate.
 */
extern struct tree_link *tree_postorder_begin(struct tree_link *tree);

/**
 * End of iteration (exclusive).
 * @param tree Tree to enumerate.
 */
extern struct tree_link *tree_postorder_end(struct tree_link *tree);

/**
 * Searches for a child of the given node, for which the specified predicate
 * evaluates to a non-zero value.
 *
 * @param node Target node to search.
 * @param predicate Predicate function to check each child.
 * @param arg Additional argument for predicate.
 *
 * @return
 *   The first child which satisfies the predicate (if any), NULL otherwise.
 */
extern struct tree_link *tree_lookup_child(struct tree_link *node,
		tree_predicate_t predicate, void *arg);

/**
 * Find element of subtree, for what specified predicate is true.
 *
 * @param tree Node, subtree of what (including itself) is tested.
 * @param predicate Predicate, with what nodes are tested.
 *
 * @return Node, for what predicate is true, or NULL, if it doesn't exist.
 */
extern struct tree_link *tree_lookup(struct tree_link *tree,
		tree_predicate_t predicate, void *arg);

extern struct tree_link *tree_children_begin(struct tree_link *tree);

extern struct tree_link *tree_children_end(struct tree_link *tree);

extern struct tree_link *tree_children_next(struct tree_link *tree);

/**
 * Iteration on tree. Elements are links (without casting from links).
 * For enumeration functions begin, end, next are used, corresponding to
 *   first element of enumeration, last(exclusively)
 *   and obtaining next element according to given current.
 */
#define tree_foreach_link(link, tree, begin, end, next) \
	__tree_foreach_link(link, MACRO_GUARD(end_link), tree, begin, end, next)

#define __tree_foreach_link(link, end_link, tree, begin, end, next) \
	for (struct tree_link *end_link = (link = begin(tree), end(tree)); \
			link != end_link; \
			link = next(link))


/**
 * Iteration on tree. Elements are links (without casting from links).
 * Calculates next element before processing of current.
 * E.g., this Allows deletion of current element.
 * For enumeration functions begin, end, next are used, corresponding to
 *   first element of enumeration, last(exclusively)
 *   and obtaining next element according to given current.
 *   'Next' function must not cause an error if it's applyed to end of enumeration.
 */
#define tree_foreach_link_safe(link, tree, begin, end, next) \
	__tree_foreach_link_safe(link, MACRO_GUARD(next_link), MACRO_GUARD(end_link), \
			tree, begin, end, next)

#define __tree_foreach_link_safe(link, next_link, end_link, tree, begin, end, next) \
	for (struct tree_link *next_link = (link = begin(tree), next(link)) \
			, *end_link = end(tree); \
		link != end_link; \
		link = next_link, next_link = next(link))

/**
 * Iteration with casting.
 * For enumeration functions begin, end, next are used, corresponding to
 *   first element of enumeration, last(exclusively)
 *   and obtaining next element according to given current.
 */
#define tree_foreach(element, tree, link_member, begin, end, next) \
	__tree_foreach(MACRO_GUARD(link), MACRO_GUARD(end_link), \
			element, tree, link_member, begin, end, next)

#define __tree_foreach(link, end_link, \
			element, tree, link_member, begin, end, next) \
	for (struct tree_link *link = begin(tree), \
			*end_link = end(tree);\
		link != end_link \
			&& (element = tree_element(link, typeof(*element), link_member)); \
		link = next(link))

/** Iterating only on children of node (not all subtree). */
#define tree_foreach_children_link(link, tree) \
	tree_foreach_link(link, tree, \
		tree_children_begin, tree_children_end, tree_children_next)

/** Iterating with casting only on children of node (not all subtree). */
#define tree_foreach_children(element, tree, link_member) \
	tree_foreach(element, tree, link_member, \
		tree_children_begin, tree_children_end, tree_children_next)

/**
 * Postorder iteration on tree.
 * Elements are links (without casting from links).
 */
#define tree_postorder_traversal_link(link, tree) \
	tree_foreach_link(link, tree, \
		tree_postorder_begin, tree_postorder_end, tree_postorder_next)

/**
 * Postorder iteration on tree.
 * Elements are links (without casting from links).
 * Calculates next element before processing of current.
 * E.g., this allows deletion of current element.
 */
#define tree_postorder_traversal_link_safe(link, tree) \
	tree_foreach_link_safe(link, tree, \
		tree_postorder_begin, tree_postorder_end, tree_postorder_next)

/** Postorder iteration with casting. */
#define tree_postorder_traversal(element, tree, link_member) \
	tree_foreach(element, tree, link_member, \
		tree_postorder_begin, tree_postorder_end, tree_postorder_next)

#endif /* UTIL_TREE_H_ */
