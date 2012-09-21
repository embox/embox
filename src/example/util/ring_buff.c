/**
 * @file
 *
 * @brief Circular buffer example
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#include <string.h>
#include <framework/example/self.h>

#include <util/ring_buff.h>

EMBOX_EXAMPLE(rbuff_example_run);

RING_BUFFER_DEF(test_buffer, int, 10);

static int rbuff_example_run(int argc, char *argv[]) {
	int a = 5;

	ring_buff_enqueue(&test_buffer, &a, 1);

	a = 6;

	ring_buff_dequeue(&test_buffer, &a, 1);

	return 0;
}
