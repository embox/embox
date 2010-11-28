/**
 * @file
 * @brief Tests queue data structure.
 *
 * @data 28.06.2010
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <queue.h>
#include <lib/list.h>

EMBOX_TEST(run);

struct my_int {
	int a;
	struct list_head list;
};

static queue_t a_queue;
static struct my_int numbers[10];

static int run(void) {
	int result = 0;
	size_t i;
	queue_init(&a_queue);

	for (i = 1; i < 10; i++) {
		TRACE("%d", queue_empty(&a_queue));
		numbers[i].a = i;
		queue_add(&numbers[i].list, &a_queue);
	}
	TRACE("\n");

	while (!queue_empty(&a_queue)) {
		TRACE("%d ", ((struct my_int*)list_entry(queue_extr(&a_queue),
							struct my_int, list))->a);
	}
	TRACE("\n");

	return result;
}
