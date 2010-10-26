/**
 * @file
 *
 * @date 28.06.2010
 * @author Dmitry Avdyukhin
 */

#include <lib/list.h>
#include <queue.h>

struct list_head* queue_extr(queue_t *a_queue) {
	struct list_head *result = a_queue->next;
	list_del(result);
	return result;
}
