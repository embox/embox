/**
 * @file
 * @brief Tree data structure.
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_H_
#define UTIL_TREE_H_

#include <util/member.h>
#include <util/list.h>

#define tree_element(link, element_type, link_member) \
	(link == NULL ? NULL \
		 : member_cast_out(link, element_type, link_member)

/*#include __impl_x(util/tree_impl.c)*/

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
 * Initialize tree link.
 * @param tree_link Link to initialize.
 */
extern struct tree_link *tree_link_init(struct tree_link *link);

/**
 * Add element to tree by adding new link into list of children of another node.
 * XXX
 * So, more than one equal subtree can be a descedant of a node (or even different nodes).
 * This isn't good and with high probability lead will to bugs, because:
 *   1) Different parent nodes - we not always know from here delete node;
 *   2) Operation with one subtree will cause effect to another clone.
 * @param parent Parent of new node.
 * @param link Added element
 */
extern void tree_add_link(struct tree_link *parent, struct tree_link *link);

/**
 * Delete element from tree. Pointers, concerned with its subtree, won't be erased.
 * Actually, this function separates node from its parent.
 * @param link Deleted element
 * @return
 * 	true, if element was in the tree before deletion.
 */
extern int tree_remove_link(struct tree_link *link);

/**
 * 'Next' link in the tree according to selected order. Used for iteration.
 * The order is: firstly recursively we enumerate all children of node,
 *   then the node itself.
 * @param link Current node.
 * @return Next node in the tree.
 */
extern struct tree_link *tree_next_link(struct tree_link *link);

/**
 * First node for enumeration.
 * @param tree Tree to enumerate.
 */
extern struct tree_link *tree_begin(struct tree_link *tree);

/** End of iteration (exclusive). */
#define tree_end(tree) ((tree)->par)

/** Iteration on tree. Elements are links (without casting from links). */
#define tree_foreach_link(link, tree) \
	for (link = tree_begin(tree); \
			link != tree_end(tree); \
			link = tree_next_link(link))

/** Iteration with casting. */
#define tree_foreach(link, element, tree, link_member) \
	for (link = tree_begin(set), element = tree_element(link, typeof(*element), link_member); \
			link != tree_end(set); \
		link = tree_next_link(link), element = tree_element(link, typeof(*element), link_member)) \

#endif /* UTIL_TREE_H_ */
