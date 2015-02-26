/**
 * @file
 * @brief Largest integral value not greater than argument
 *
 * @date 13.09.11
 * @author Anton Bondarev
 */

#include <math.h>
/*
double floor(double x) {
	double rounded;

	rounded = round(x);
	return rounded <= x ? rounded : rounded - 1.0;
}
*/
float floorf(float x) {
	float rounded;

	rounded = round(x);
	return rounded <= x ? rounded : rounded - 1.0F;
}

long double floorl(long double x) {
	long double rounded;

	rounded = round(x);
	return rounded <= x ? rounded : rounded - 1.0L;
}
