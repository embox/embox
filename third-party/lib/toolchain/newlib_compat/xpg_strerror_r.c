/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.05.25
 */

#include <stddef.h>
#include <string.h>

int __xpg_strerror_r(int errnum, char *strerrbuf, size_t buflen) {
	return strerror_r(errnum, strerrbuf, buflen);
}
