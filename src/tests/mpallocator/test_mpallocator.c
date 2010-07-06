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

#include <kernel/mm/mpallocator.h>

EMBOX_TEST(run);

static int run(void) {
	void *ptr;
	printf("Special for Michail: ");
	ptr = mpalloc(4);
	printf("was selected ptr[%d]; ", (unsigned int)ptr);
	mpfree(ptr);
	printf("was free. \n");
	return 0;
}

