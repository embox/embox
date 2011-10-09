/**
 * @file
 * @brief Binary not-balanced tree data structure. There can be not more than one equal element in structure.
 *
 * @date Oct 1, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_SET_H_
#define UTIL_SET_H_

#include <assert.h>
#include <util/member.h>

struct set_link;

struct set;

#include __impl_x(util/set_impl.c)

/**
 * Compare two elements of set.
 *
 * @param first
 * @param second
 * @return
 *   The result of comparison of the @a first and the @a second.
 */
//typedef int (*set_comparator_t)(struct set_link *first, struct set_link *second);

#define set_element(link, element_type, link_member) \
	member_cast_out(link, element_type, link_member)

/**
 * Initialize set.
 * @param set Set to initialize.
 */
extern struct set *set_init(struct set *set);

/**
 * Initialize set link.
 * @param set Link to initialize.
 */
extern struct set_link *set_link_init(struct set_link *link);

/**
 * Check whether the set is empty.
 * @param set
 * @return true iff set is empty.
 */
extern int set_empty(struct set *set);

/**
 * Find element in the set.
 * Element is considered to be found if result of compare function is zero.
 * @param set Tree, where element is searched
 * @param link Element, equal to which is found.
 * @param compare Compare function
 * @return
 * 	NULL, if there is no such element in set.
 *	Pointer to this element otherwise.
 */
extern struct set_link *set_find_link(struct set *set,
		struct set_link *link, set_comparator_t compare);

/**
 * Add element to set.
 * If there was an element equaled to added, it would be replaced.
 * @param set Tree, element is inserted into
 * @param link Added element
 * @param compare Compare function
 * @return
 * 	true, if element wasn't in set.
 */
extern int set_add_link(struct set *set,
		struct set_link *link, set_comparator_t compare);

/**
 * Delete element from set, which compare result with given element is zero.
 * @param set Tree, element is deleted from
 * @param link Deleted element
 * @param compare Compare function
 * @return
 * 	true, if element was in set before deletion.
 */
extern int set_remove_link(struct set* set,
		struct set_link *link, set_comparator_t compare);

/**
 * Return the most left element of the set.
 * According to set structure, this element must be the smallest one.
 * @param set
 * @return The smallest element of the set.
 */
extern struct set_link *set_min_link(struct set *set);

/**
 * Return the most right element of the set.
 * According to set structure, this element must be the greatest one.
 * @param set
 * @return The greatest element of the set.
 */
extern struct set_link *set_max_link(struct set *set);

#endif /* UTIL_SET_H_ */
