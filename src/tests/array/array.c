/**
 * @file
 * @brief Testing array utilities
 *
 * @date 14.03.11
 * @author Nikolay Korotky
 */

#include <embox/test.h>
#include <string.h>
#include <util/list.h>
#include <util/array.h>
#include <test/tools.h>

EMBOX_TEST(run);

static const char *test_array[] = { "foo", "bar" };

static int run(void) {
	char buf[5];
	int count = 0;
	const char *tmp;

	array_static_foreach(tmp, test_array) {
		strncpy(buf, tmp, sizeof(buf));
		count++;
	}
	return count != 2;
}

