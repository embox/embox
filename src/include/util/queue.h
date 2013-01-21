/**
 * @file
 * @brief 'queue' interface
 *
 * @date 20.02.12
 * @author Ilia Vaprol
 */

#ifndef UTIL_QUEUE_H_
#define UTIL_QUEUE_H_

#include <stdlib.h>

/**
 * Forwarding queue structure declaration. You do not allow to use
 * field of this structure outside of queue implementation library
 */
struct queue;

/**
 * Create empty queue.
 *
 * @return pointer to created queue or NULL
 */
extern struct queue * queue_create(void);

/**
 * Checks whether there are any items in the queue or not.
 *
 * @param q existing queue with items (or without)
 *
 * @return 1 if queue is empty and zero if not
 */
extern int queue_empty(struct queue *q);

/**
 * Get a number of items in queue.
 *
 * @param q existing queue with items (or without)
 *
 * @return number of items in queue
 */
extern size_t queue_size(struct queue *q);

/**
 * Get a first element in queue
 *
 * @param q existing queue with items
 *
 * @return first element in current queue
 */
void * queue_front(struct queue *q);

/**
 * Get a last element in queue
 *
 * @param q existing queue with items
 *
 * @return last element in current queue
 */
void * queue_back(struct queue *q);

/**
 * Insert element into the tail of queue
 *
 * @param q existing queue with items (or without)
 * @param val element for insertion to current queue
 *
 * @return error code
 * @retval ENOERR if ok
 * @retval -ENOMEM if there is no memory
 */
int queue_push(struct queue *q, void *val);

/**
 * Delete element from the head of queue
 *
 * @param q existing queue with items
 */
void queue_pop(struct queue *q);

/**
 * Delete all elements and release the memory of a queue
 *
 * @param q existing queue with items (or without)
 */
void queue_destroy(struct queue *q);

#endif /* UTIL_QUEUE_H_ */
