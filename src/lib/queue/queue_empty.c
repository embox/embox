/**
 * @file
 *
 * @date 28.06.2010
 * @author Dmitry Avdyukhin
 */

#include <lib/list.h>
#include <queue.h>

bool queue_empty(queue_t *a_queue) {
	return list_empty(a_queue);
}
