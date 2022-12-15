/**
 * @file
 *
 * @date Dec 15, 2022
 * @author Anton Bondarev
 */

#include <math.h>

long double ceill(long double x) {
	return floorl(x + 1);
}

double ceil(double x) {
	return ceill(x);
}

float ceilf(float x) {
	return ceill(x);
}
