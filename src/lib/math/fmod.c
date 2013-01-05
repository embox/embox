/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

double fmod(double x, double y) {
	double quot;

	quot = x / y;
	return x - (signbit(quot) ? ceil(quot) : floor(quot)) * y;
}

float fmodf(float x, float y) {
	float quot;

	quot = x / y;
	return x - (signbitf(quot) ? ceilf(quot) : floorf(quot)) * y;
}

long double fmodl(long double x, long double y) {
	long double quot;

	quot = x / y;
	return x - (signbitl(quot) ? ceill(quot) : floorl(quot)) * y;
}
