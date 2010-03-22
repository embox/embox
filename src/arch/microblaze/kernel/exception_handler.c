/**
 * @file
 *
 * @date 17.03.2010
 * @author Anton Bondarev
 */
#include <types.h>
#include <asm/traps_core.h>
#include <test/testtraps_core.h>

softtrap_handler_t test_handler[MAX_SOFTTRAP_NUMBER];

void exception_handler(int number) {
	if (NULL == test_handler)
		return;
	test_handler[number](0,0);
}
