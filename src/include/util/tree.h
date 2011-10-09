/**
 * @file
 * @brief Tree data structure.
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_H_
#define UTIL_TREE_H_

#include <assert.h>
#include <util/member.h>
#include <util/list.h>

#define tree_element(link, element_type, link_member) \
	member_cast_out(link, element_type, link_member)

#include __impl_x(util/tree_impl.c)

struct tree_link;

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

#endif /* UTIL_TREE_H_ */
