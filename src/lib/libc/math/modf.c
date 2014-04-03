/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stddef.h>
#include <math.h>

double modf(double x, double *iptr) {
	double fract_part;

	fract_part = fmod(x, 1.0);

	assert(iptr != NULL);
	*iptr = copysign(x - fract_part, x);

	return fract_part;
}

float modff(float x, float *iptr) {
	float fract_part;

	fract_part = fmodf(x, 1.0F);

	assert(iptr != NULL);
	*iptr = copysignf(x - fract_part, x);

	return fract_part;
}

long double modfl(long double x, long double *iptr) {
	long double fract_part;

	fract_part = fmodl(x, 1.0L);

	assert(iptr != NULL);
	*iptr = copysignl(x - fract_part, x);

	return fract_part;
}
