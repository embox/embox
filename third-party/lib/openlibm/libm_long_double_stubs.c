#include <math.h>

/* FIXME Here should be more tricky method to determine long double == double.
 * But currently, I derived this check from openlibm/src/Make.files 
 *
 * https://github.com/JuliaMath/openlibm/issues/232
 */
#if !defined(i386) && !defined (__aarch64__)
long double roundl(long double x) {
	return round(x);
}

/* TODO Add other functions like sinl() etc. */

#endif
