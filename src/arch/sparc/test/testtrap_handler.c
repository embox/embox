/**
 * @file
 *
 * @date 25.03.2010
 * @author Alexander Batyukov
 */

#include <test/testtraps_core.h>
#include <stdio.h>

trap_handler_t test_handler[CONFIG_MAX_SOFTTRAP_NUMBER];

void test_soft_trap_handler(uint8_t tt, uint32_t *data) {
	if (NULL == test_handler)
		return;
	test_handler[tt - MIN_SOFT_TRAP_NUMBER](tt, (void *)data);
}
