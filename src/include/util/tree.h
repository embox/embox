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
#include <util/list.h>

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
	struct list children;

	/** List link inside of list of children. */
	struct list_link list_link;
};

/**
 * Type of functions, returning true or false for given tree_link.
 */
typedef int (*tree_link_predicate_t)(struct tree_link *link);

/*
 * Initialize tree link.
 * @param tree_link Link to initialize.
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
 * Find children of given node, for what specified predicate is true.
 *
 * @param node Node, what children are tested.
 * @param predicate Predicate, with what nodes are tested.
 *
 * @return Child, for what predicate is true, or NULL, if it doesn't exist.
 */
extern struct tree_link *tree_children_find(struct tree_link *node,
		tree_link_predicate_t predicate);

/**
 * Find element of subtree, for what specified predicate is true.
 *
 * @param tree Node, subtree of what (including itself) is tested.
 * @param predicate Predicate, with what nodes are tested.
 *
 * @return Node, for what predicate is true, or NULL, if it doesn't exist.
 */
extern struct tree_link *tree_find(struct tree_link *tree,
		tree_link_predicate_t predicate);

/**
 * Iteration on tree. Elements are links (without casting from links).
 * For enumeration functions begin, end, next are used, corresponding to
 *   first element of enumeration, last(exclusively)
 *   and obtaining next element according to given current.
 */
#define tree_foreach_link(link, tree, begin, end, next) \
	for (link = begin(tree); link != end(tree); link = next(link))

/**
 * Iteration on tree. Elements are links (without casting from links).
 * Calculates next element before processing of current.
 * E.g., this Allows deletion of current element.
 * For enumeration functions begin, end, next are used, corresponding to
 *   first element of enumeration, last(exclusively)
 *   and obtaining next element according to given current.
 *   'Next' function must not cause an error if it's applyed to end of enumeration.
 */
#define tree_foreach_link_safe(link, next_link, tree, begin, end, next) \
	for (link = begin(tree), next_link = next(link); \
		link != end(tree); \
		link = next_link, next_link = next(link))

/**
 * Iteration with casting.
 * For enumeration functions begin, end, next are used, corresponding to
 *   first element of enumeration, last(exclusively)
 *   and obtaining next element according to given current.
 */
#define tree_foreach(link, element, tree, link_member, begin, end, next) \
	for (link = begin(tree), \
			element = tree_element(link, typeof(*element), link_member); \
		link != end(tree); \
		link = next(link), \
			element = tree_element(link, typeof(*element), link_member))

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
#define tree_postorder_traversal_link_safe(link, next_link, tree) \
	tree_foreach_link_safe(link, next_link, tree, \
		tree_postorder_begin, tree_postorder_end, tree_postorder_next)

/** Postorder iteration with casting. */
#define tree_postorder_traversal(link, element, tree, link_member) \
	tree_foreach(link, element, tree, link_member, \
		tree_postorder_begin, tree_postorder_end, tree_postorder_next)

#endif /* UTIL_TREE_H_ */
