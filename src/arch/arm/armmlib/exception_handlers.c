/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.03.2015
 */

#include <kernel/panic.h>

void undef_handler(void) {
	panic("\n%s\n", __func__);
}

void hard_fault(void) {
	panic("\n%s\n", __func__);
}
