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

/**
 * A link which has to be embedded into each element of the set.
 *
 * Usage:
 * @code
 *  struct my_element {
 *  	...
 *  	struct set_link my_link;
 *  	...
 *  };
 * @endcode
 *
 */
struct set_link {
	struct set_link *left, *right, *par;
};

/**
 * Type, representing set.
 */
struct set {
	struct set_link *root;
};

/*#include __impl_x(util/set_impl.c)*/

/**
 * Compare two elements of set.
 *
 * @param first
 * @param second
 * @return
 *   The result of comparison of the @a first and the @a second.
 */
typedef int (*set_comparator_t)(struct set_link *first, struct set_link *second);

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
#define set_element(link_ptr, element_t, m_link) \
	((link_ptr) == NULL ? NULL : \
	 	member_cast_out(link_ptr, element_t, m_link))

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

/** First element in iteration (inclusively). */
#define set_begin(set) (set_min_link(set))

/** First element in back iteration (inclusively). */
#define set_rbegin(set) (set_max_link(set))

/** Last element in iteration (exclusively). */
#define set_end(set) NULL

/**
 * Return next link. Resulting link is the smallest one, greater than argument.
 * Can be used for iteration.
 * @param link Link, next afther that we want to find. Mustn't be NULL.
 * @return Next link in the set.
 */
extern struct set_link *set_next_link(struct set_link *link);

/**
 * Return prev link. Resulting link is the greatest one, smaller than argument.
 * Can be used for iteration.
 * @param link Link, prev before that we want to find. Mustn't be NULL.
 * @return Prev link in the set.
 */
extern struct set_link *set_prev_link(struct set_link *link);

/** Foreach variations. */

/** Forward iteration. Elements are links (without casting from links). */
#define set_foreach_link(link, set) \
	for (link = set_begin(set); \
			link != set_end(set); \
			link = set_next_link(link))

/** Backward iteration. Elements are links (without casting from links). */
#define set_foreach_back_link(link, set) \
	for (link = set_rbegin(set); \
			link != set_end(set); \
			link = set_prev_link(link))

/** Forward iteration with casting. */
#define set_foreach(link, element, set, link_member) \
	for (link = set_begin(set), element = set_element(link, typeof(*(element)), link_member); \
			link != set_end(set); \
		link = set_next_link(link), element = set_element(link, typeof(*(element)), link_member)) \

/** Backward iteration with casting. */
#define set_foreach_back(link, element, set, link_member) \
	for (link = set_rbegin(set), element = set_element(link, typeof(*element), link_member); \
			link != set_end(set); \
		link = set_prev_link(link), element = set_element(link, typeof(*element), link_member)) \


#endif /* UTIL_SET_H_ */
