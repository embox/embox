/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

extern float __remsf3(float a, float b);
extern double __remdf3(double a, double b);
extern long double __remtf3(long double a, long double b);

/*
double fmod(double x, double y) {
	double fract_part;

	fract_part = __remdf3(x, y);

	return signbit(x) ^ signbit(fract_part)
			? copysign(y, x) + fract_part
			: fract_part;
}
*/

float fmodf(float x, float y) {
	float fract_part;

	fract_part = __remsf3(x, y);

	return signbitf(x) ^ signbitf(fract_part)
			? copysignf(y, x) + fract_part
			: fract_part;
}

long double fmodl(long double x, long double y) {
	long double fract_part;

	fract_part = __remtf3(x, y);

	return signbitl(x) ^ signbitl(fract_part)
			? copysignl(y, x) + fract_part
			: fract_part;
}
