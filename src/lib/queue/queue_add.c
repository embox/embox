/**
 * @file
 *
 * @date 28.06.2010
 * @author Dmitry Avdyukhin
 */

#include <lib/list.h>
#include <queue.h>

void queue_add(struct list_head *list, queue_t *a_queue) {
	list_add_tail(list, a_queue);
}
