/**
 * @file
 * @brief test page allocator
 * @details Some random tests for page allocator
 *
 * @date 28.07.10
 * @author Fedor Burdun
 */

#include <embox/test.h>
#include <stdio.h>

#include <lib/multipage_alloc.h>

EMBOX_TEST(run);

static int run(void) {
	printf("Special for Michail: ");
	void * ptr = multipage_alloc(4);
	printf("was selected ptr[%d]; ", ptr);
	multipage_free(ptr);
	printf("was free. \n");
	return 0;
}

