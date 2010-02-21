/**
 * @file
 * @brief No-op function returning -ENOSYS.
 *
 * @date 18.02.2010
 * @author Eldar Abusalimov
 */

#include <errno.h>

int nosys(void) {
	return -ENOSYS;
}
