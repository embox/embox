/**
 * @file stpcpy.c
 * @brief POSIX function to copy string
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.10.2018
 * 
 * @author Aleksey Zhmulin
 * @note arm-none-eabi-gcc (12.2.1) assembler error
 * when compiling with -fsanitize=undefined
 * "Error: expected comma after name `' in .size directive"
 */

#include <string.h>

#ifdef __clang__
#define ATTR_NO_SANITIZE 
#elif __GNUC__
#define ATTR_NO_SANITIZE  __attribute__((no_sanitize_undefined))
#endif 
char *ATTR_NO_SANITIZE
stpcpy(char *dest, const char *src) {
	while ((*dest = *src++)) {
		++dest;
	}

	return dest;
}
