/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

/**
 * TODO see modf.c about #if 0
 */

#if 0
extern float __remsf3(float a, float b);
extern double __remdf3(double a, double b);
extern long double __remtf3(long double a, long double b);
#endif

double fmod(double x, double y) {
#if 0
	double fract_part;

	fract_part = __remdf3(x, y);

	return signbit(x) ^ signbit(fract_part)
			? copysign(y, x) + fract_part
			: fract_part;
#else
	double int_part;

	modf(x / y, &int_part);
	return x - int_part * y;
#endif
}

float fmodf(float x, float y) {
#if 0
	float fract_part;

	fract_part = __remsf3(x, y);

	return signbitf(x) ^ signbitf(fract_part)
			? copysignf(y, x) + fract_part
			: fract_part;
#else
	float int_part;

	modff(x / y, &int_part);
	return x - int_part * y;
#endif
}

long double fmodl(long double x, long double y) {
#if 0
	long double fract_part;

	fract_part = __remtf3(x, y);

	return signbitl(x) ^ signbitl(fract_part)
			? copysignl(y, x) + fract_part
			: fract_part;
#else
	long double int_part;

	modfl(x / y, &int_part);
	return x - int_part * y;
#endif
}
