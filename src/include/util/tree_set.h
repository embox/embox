/**
 * @file
 * @brief Binary not-balanced tree data structure.
 * There can be not more than one equal element in structure.
 *
 * @date 01.10.11
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_SET_H_
#define UTIL_TREE_SET_H_

#include <assert.h>
#include <util/member.h>

/** Color of node in RB-tree. */
enum tree_set_color {TREE_SET_NONE, TREE_SET_BLACK, TREE_SET_RED};

/**
 * A link which has to be embedded into each element of the tree_set.
 *
 * Usage:
 * @code
 *  struct my_element {
 *  	...
 *  	struct tree_set_link my_link;
 *  	...
 *  };
 * @endcode
 *
 */
struct tree_set_link {
	struct tree_set_link *left, *right, *par;
	enum tree_set_color color;
};

/**
 * Type, representing tree_set.
 */
struct tree_set {
	struct tree_set_link *root;
};

/*#include __impl_x(util/tree_set_impl.c)*/

/**
 * Compare two elements of tree_set.
 *
 * @param first
 * @param second
 * @return
 *   The result of comparison of the @a first and the @a second.
 */
typedef int (*tree_set_comparator_t)(struct tree_set_link *first, struct tree_set_link *second);

/**
 * Dispose element, related with specified link. Used in clear function.
 *
 * @param link Link to be disposed.
 */
typedef void (*tree_set_dispose_t)(struct tree_set_link *link);

/* Cast between link member and the element. */

/**
 * Gets a pointer to an element corresponding to a given link.
 *
 * @param link_ptr
 *   The pointer to the member being casted. Can not be null.
 * @param element_t
 *   The type of the element.
 * @param m_link
 *   The name of the link member within the element.
 * @return
 *   @a element_t * pointer.
 */
#define tree_set_element(link_ptr, element_t, m_link) \
	((link_ptr) == NULL ? NULL : \
	 	member_cast_out(link_ptr, element_t, m_link))

/**
 * Initialize tree_set.
 * @param tree_set Set to initialize.
 */
extern struct tree_set *tree_set_init(struct tree_set *tree_set);

/**
 * Initialize tree_set link.
 * @param tree_set Link to initialize.
 */
extern struct tree_set_link *tree_set_link_init(struct tree_set_link *link);

/**
 * Check whether the tree_set is empty.
 * @param tree_set
 * @return true iff tree_set is empty.
 */
extern int tree_set_empty(struct tree_set *tree_set);

/**
 * Find element in the tree_set.
 * Element is considered to be found if result of compare function is zero.
 * @param tree_set Tree, where element is searched
 * @param link Element, equal to which is found.
 * @param compare Compare function
 * @return
 * 	NULL, if there is no such element in tree_set.
 *	Pointer to this element otherwise.
 */
extern struct tree_set_link *tree_set_find_link(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare);

/**
 * Add element to tree_set.
 * If there was an element equaled to added, it would be replaced.
 * @param tree_set Tree, element is inserted into
 * @param link Added element
 * @param compare Compare function
 * @return
 * 	true, if element wasn't in tree_set.
 */
extern int tree_set_add_link(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare);

/**
 * Delete element from tree_set, which compare result with given element is zero.
 * @param tree_set Tree, element is deleted from
 * @param link Deleted element
 * @param compare Compare function
 * @return
 * 	true, if element was in tree_set before deletion.
 */
extern int tree_set_remove_link(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare);

/**
 * Delete all elements from set. Applies dispose function to each element.
 *
 * @param set Set to clear
 * @param dispose Dispose function, applied to each element of set after deletion.
 */
extern void tree_set_clear(struct tree_set *set, tree_set_dispose_t dispose);

/**
 * Return the most left element of the tree_set.
 * According to tree_set structure, this element must be the smallest one.
 * @param tree_set
 * @return The smallest element of the tree_set.
 */
extern struct tree_set_link *tree_set_min_link(struct tree_set *tree_set);

/**
 * Return the most right element of the tree_set.
 * According to tree_set structure, this element must be the greatest one.
 * @param tree_set
 * @return The greatest element of the tree_set.
 */
extern struct tree_set_link *tree_set_max_link(struct tree_set *tree_set);

/** First element in iteration (inclusively). */
#define tree_set_begin(tree_set) (tree_set_min_link(tree_set))

/** First element in back iteration (inclusively). */
#define tree_set_rbegin(tree_set) (tree_set_max_link(tree_set))

/** Last element in iteration (exclusively). */
#define tree_set_end(tree_set) NULL

/**
 * Return next link. Resulting link is the smallest one, greater than argument.
 * Can be used for iteration.
 * @param link Link, next afther that we want to find. Mustn't be NULL.
 * @return Next link in the tree_set.
 */
extern struct tree_set_link *tree_set_next_link(struct tree_set_link *link);

/**
 * Return prev link. Resulting link is the greatest one, smaller than argument.
 * Can be used for iteration.
 * @param link Link, prev before that we want to find. Mustn't be NULL.
 * @return Prev link in the tree_set.
 */
extern struct tree_set_link *tree_set_prev_link(struct tree_set_link *link);

/** Foreach variations. */

/** Forward iteration. Elements are links (without casting from links). */
#define tree_set_foreach_link(link, tree_set) \
	for (link = tree_set_begin(tree_set); \
			link != tree_set_end(tree_set); \
			link = tree_set_next_link(link))

/** Backward iteration. Elements are links (without casting from links). */
#define tree_set_foreach_back_link(link, tree_set) \
	for (link = tree_set_rbegin(tree_set); \
			link != tree_set_end(tree_set); \
			link = tree_set_prev_link(link))

/** Forward iteration with casting. */
#define tree_set_foreach(link, element, tree_set, link_member) \
	for (link = tree_set_begin(tree_set), \
		element = tree_set_element(link, typeof(*(element)), link_member); \
		link != tree_set_end(tree_set); \
		link = tree_set_next_link(link), \
		element = tree_set_element(link, typeof(*(element)), link_member)) \

/** Backward iteration with casting. */
#define tree_set_foreach_back(link, element, tree_set, link_member) \
	for (link = tree_set_rbegin(tree_set), \
		element = tree_set_element(link, typeof(*element), link_member); \
		link != tree_set_end(tree_set); \
		link = tree_set_prev_link(link), \
		element = tree_set_element(link, typeof(*element), link_member)) \

#endif /* UTIL_TREE_SET_H_ */
