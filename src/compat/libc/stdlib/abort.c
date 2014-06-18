/**
 * @file
 * @brief
 * @date 12.07.12
 * @author Ilia Vaprol
 */

#include <kernel/panic.h>

void abort(void) {
	panic("Aborted");
}
