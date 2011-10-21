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

#include <util/circular_buffer.h>

EMBOX_EXAMPLE(circular_buffer_ex_run);

CIRCULAR_BUFFER_DEF(test_buffer, int, 10);

static int circular_buffer_ex_run(int argc, char *argv[]) {
	int a = 5;

	c_buf_add(&test_buffer, a);

	a = 6;

	c_buf_get(&test_buffer, a);

	return 0;
}

