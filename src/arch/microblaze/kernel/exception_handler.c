/**
 * @file
 *
 * @date 17.03.2010
 * @author Anton Bondarev
 */
#include <types.h>
#include <asm/test/testtraps_core.h>
softtrap_handler test_handler;
#include <common.h>
void exception_handler(void) {
	if (NULL == test_handler)
		return;
	test_handler(0);
}
