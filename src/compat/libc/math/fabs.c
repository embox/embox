/**
 * @file
 * @brief
 *
 * @date 07.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

#if 0
double fabs(double x) {
	return signbit(x) ? -x : x;
}
#endif

float fabsf(float x) {
	return signbitf(x) ? -x : x;
}

long double fabsl(long double x) {
	return signbitl(x) ? -x : x;
}
