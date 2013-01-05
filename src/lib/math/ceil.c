/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

double ceil(double x) {
	double int_part;

    return modf(x, &int_part) > 0.0 ? int_part + 1.0 : int_part;
}

float ceilf(float x) {
	float int_part;

    return modff(x, &int_part) > 0.0 ? int_part + 1.0 : int_part;
}

long double ceill(long double x) {
	long double int_part;

    return modfl(x, &int_part) > 0.0 ? int_part + 1.0 : int_part;
}
