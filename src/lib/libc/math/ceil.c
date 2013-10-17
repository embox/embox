/**
 * @file
 * @brief Smaller integral value not less than argument
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

double ceil(double x) {
	double rounded;

	rounded = round(x);
	return rounded < x ? rounded + 1.0 : rounded;
}

float ceilf(float x) {
	float rounded;

	rounded = roundf(x);
	return rounded < x ? rounded + 1.0F : rounded;
}

long double ceill(long double x) {
	long double rounded;

	rounded = roundf(x);
	return rounded < x ? rounded + 1.0L : rounded;
}
