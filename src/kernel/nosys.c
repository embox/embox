/**
 * @file
 * @brief No-op function returning -ENOSYS.
 *
 * @date 18.02.10
 * @author Eldar Abusalimov
 */

#include <errno.h>

/**
 * No operation function returning -ENOSYS.
 * @return -ENOSYS
 */
int nosys(void) {
	return -ENOSYS;
}
