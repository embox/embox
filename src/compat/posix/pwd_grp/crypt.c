/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.05.2014
 */

#include <string.h>

char *crypt(const char *key, const char *salt) {
	static char _crypt_buf[64];
	return strncpy(_crypt_buf, key, sizeof(_crypt_buf) - 1);
}
