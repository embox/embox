/**
 * @file stpcpy.c
 * @brief POSIX function to copy string
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.10.2018
 */

#include <string.h>

/**
 * @brief Copy string from dest to src
 *
 * @param dest
 * @param src
 *
 * @note  Arrays should not overlap
 * @note  This functions differs from strcpy() in return value
 *
 * @return Pointer to terminating NULL
 */
char *stpcpy(char * dest, const char * src) {
	char *cp = dest;
	while ((*cp++ = *src++));
	return dest - 1;
}
