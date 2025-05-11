/**
 * @file
 * @brief arc4random function stub.
 *
 * @see stdlib.h
 *
 * @date 11.05.25
 * @author Dmitry Pilyuk
 */

#include <errno.h>
#include <stdlib.h>

uint32_t arc4random(void) {
	SET_ERRNO(ENOSYS);
	return 0;
}