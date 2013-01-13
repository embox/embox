/**
 * @file
 * @brief
 *
 * @date 18.12.12
 * @author Ilia Vaprol
 */

#include <math.h>

double copysign(double x, double y) {
	return signbit(x) ^ signbit(y) ? -x : x;
}

float copysignf(float x, float y) {
	return signbitf(x) ^ signbitf(y) ? -x : x;
}

long double copysignl(long double x, long double y) {
	return signbitl(x) ^ signbitl(y) ? -x : x;
}
