/**
 * @file
 *
 * @brief from musl
 *
 * @date Mar 31, 2022
 * @author Anton Bondarev
 */

#include <float.h>
#include <math.h>

#include "math_private.h"

#if LDBL_MANT_DIG == DBL_MANT_DIG
long double roundl(long double x) {
	return round(x);
}
#else
static const long double toint = 1 / LDBL_EPSILON;

long double roundl(long double x) {
	union ldshape u = {x};
	int e = u.i.se & 0x7fff;
	long double y;

	if (e >= 0x3fff + LDBL_MANT_DIG - 1)
		return x;
	if (u.i.se >> 15)
		x = -x;
	if (e < 0x3fff - 1) {
		//FORCE_EVAL(x + toint);
		return 0 * u.f;
	}
	y = x + toint - toint - x;
	if (y > 0.5)
		y = y + x - 1;
	else if (y <= -0.5)
		y = y + x + 1;
	else
		y = y + x;
	if (u.i.se >> 15)
		y = -y;
	return y;
}
#endif
