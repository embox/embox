/**
 * @file
 * @brief Realization of queue data structure.
 *
 * @date 28.06.2010
 * @author Dmitry Avdyukhin
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <lib/list.h>
#include <types.h>

typedef struct list_head queue_t;

/**
 * Add new element in queue.
 *
 * @param a_queue queue, which new element is added in.
 * @param list new element.
 */
extern void queue_add(struct list_head *list, queue_t *a_queue);

/**
 * Extract first element from queue.
 *
 * @param a_queue queue, whose element is extracted.
 * @return first element of queue
 */
extern struct list_head* queue_extr(queue_t *a_queue);

/**
 * Check if queue is empty.
 *
 * @param a_queue queue to check.
 * @return if queue is empty.
 */
extern bool queue_empty(queue_t *a_queue);

/**
 * Initialize queue.
 *
 * @param a_queue queue to init.
 */
extern void queue_init(queue_t *a_queue);

#endif /* QUEUE_H_ */
