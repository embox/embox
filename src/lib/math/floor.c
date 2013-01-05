/**
 * @file
 * @brief Largest integral value not greater than argument
 *
 * @date 13.09.11
 * @author Anton Bondarev
 */

#include <math.h>

double floor(double x) {
	double int_part;

    return modf(x, &int_part) < 0.0 ? int_part - 1.0 : int_part;
}

float floorf(float x) {
	float int_part;

    return modff(x, &int_part) < 0.0 ? int_part - 1.0 : int_part;
}

long double floorl(long double x) {
	long double int_part;

    return modfl(x, &int_part) < 0.0 ? int_part - 1.0 : int_part;
}
