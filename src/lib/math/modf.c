/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdlib.h>
#include <math.h>

double modf(double x, double *ipart) {
	double fpart;

	fpart = fmod(x, 1.0);

	assert(ipart != NULL);
	*ipart = x - fpart;

	return fpart;
}

float modff(float x, float *ipart) {
	float fpart;

	fpart = fmodf(x, 1.0);

	assert(ipart != NULL);
	*ipart = x - fpart;

	return fpart;
}

long double modfl(long double x, long double *ipart) {
	long double fpart;

	fpart = fmodl(x, 1.0);

	assert(ipart != NULL);
	*ipart = x - fpart;

	return fpart;
}
