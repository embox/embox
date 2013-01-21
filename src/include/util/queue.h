/**
 * @file
 * @brief 'queue' interface
 *
 * @date 20.01.13
 * @author Ilia Vaprol
 */

#ifndef UTIL_QUEUE_H_
#define UTIL_QUEUE_H_

#include <stdlib.h>
#include <util/member.h>

#include __impl_x(util/queue_impl.h)

/* Types used for the list itself and for elements linkage. */

/**
 * A head of the list.
 *
 * @see queue_init()
 * @see QUEUE_INIT()
 * @see QUEUE_DEF()
 *   A handy macro for defining variables of such type.
 */
struct queue;

/**
 * A link which has to be embedded into each element of the queue.
 *
 * Usage:
 * @code
 *  struct my_element {
 *  	...
 *  	struct queue_link my_link;
 *  	...
 *  };
 * @endcode
 *
 * @see queue_link_init()
 * @see queue_LINK_INIT()
 */
struct queue_link;

/* Cast between link member and the element. */

/**
 * Gets a pointer to an element corresponding to a given link.
 *
 * @param link_ptr
 *   The pointer to the member being casted. Must not be null.
 * @param element_t
 *   The type of the element.
 * @param m_link
 *   The name of the link member within the element.
 * @return
 *   A element_t * pointer.
 */
#define queue_element(link_ptr, element_t, m_link) \
	member_cast_out(link_ptr, element_t, m_link)

/* Static initializers. */

/**
 * @param queue
 *   Pointer to the list to initialize.
 */
#define QUEUE_INIT(queue) \
	  __QUEUE_INIT(queue)

/**
 * @param link
 *   Pointer to the link.
 */
#define QUEUE_LINK_INIT(link) \
	  __QUEUE_LINK_INIT(link)

/**
 * @param list_nm
 *   Variable name to use.
 */
#define QUEUE_DEF(queue_nm) \
	struct queue queue_nm = QUEUE_INIT(&queue_nm)

/* Initialization functions. */

/**
 * @param queue
 *   Pointer to the queue being initialized.
 * @return
 *   The argument.
 */
extern struct queue * queue_init(struct queue *queue);

/**
 * @param link
 *   Pointer to the link.
 * @return
 *   The argument.
 */
extern struct queue_link * queue_link_init(struct queue_link *link);

/**
 * Checks whether there are any items in the queue or not.
 *
 * @param queue
 *   Pointer to the queue.
 * @return
 *   Empty or not.
 */
extern int queue_is_empty(struct queue *queue);

/**
 * Get a number of items in queue.
 *
 * @param queue
 *   Pointer to the queue.
 * @return
 *   The size of queue.
 */
extern size_t queue_size(struct queue *queue);

/* Getting a first element from its queue. */

#define queue_front(linkage_t, queue) \
	member_to_object_or_null(queue_front_link(queue), linkage_t)

#define queue_front_element(queue, element_type, link_member) \
	member_cast_out_or_null(queue_front_link(queue), element_type, link_member)

extern struct queue_link * queue_front_link(struct queue *queue);

/* Getting a last element from its queue. */

#define queue_back(linkage_t, queue) \
	member_to_object_or_null(queue_back_link(queue), linkage_t)

#define queue_back_element(queue, element_type, link_member) \
	member_cast_out_or_null(queue_back_link(queue), element_type, link_member)

extern struct queue_link * queue_back_link(struct queue *queue);

/* Adding an element at the queue ends. */

#define queue_push(linkage_t, element, queue) \
	queue_push_link(member_of_object(element, linkage_t), queue)

#define queue_push_element(element, queue, link_member) \
	queue_push_link(member_cast_in(element, link_member), queue)

extern void queue_push_link(struct queue_link *new_link, struct queue *queue);

/* Removing an element from the begin of queue. */

#define queue_pop(linkage_t, queue) \
	member_to_object_or_null(queue_pop_link(queue), linkage_t)

#define queue_pop_element(queue, element_type, link_member) \
	member_cast_out_or_null(queue_pop_link(queue), element_type, link_member)

extern struct queue_link * queue_pop_link(struct queue *queue);

#endif /* UTIL_QUEUE_H_ */
