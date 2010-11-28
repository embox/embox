/**
 * @file
 *
 * @date 28.06.2010
 * @author Dmitry Avdyukhin
 */

#include <lib/list.h>
#include <queue.h>

void queue_init(queue_t *a_queue) {
	INIT_LIST_HEAD(a_queue);
}
