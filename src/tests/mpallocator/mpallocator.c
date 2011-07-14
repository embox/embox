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
#include <mem/pagealloc/mpallocator.h>

EMBOX_TEST(run);

static int run(void) {
	void *ptr;
	//TRACE("\n\e[1;31mSpecial for Michail: \e[0;0m");
	/* multipage_info(); */
	ptr = mpalloc(1);
	TRACE("was selected ptr: %d ; ", (unsigned int)ptr);
	/* multipage_info(); */
	mpfree(ptr);
	TRACE("was free. ");
	return 0;
}
