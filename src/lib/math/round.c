/**
 * @file
 * @brief
 *
 * @date 07.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

double round(double x) {
	double int_part, frac_part;

    frac_part = modf(x, &int_part);
    return frac_part <= -0.5 ? int_part - 1.0
            : frac_part >= 0.5 ? int_part + 1.0
            : int_part;
}

float roundf(float x) {
	float int_part, frac_part;

    frac_part = modff(x, &int_part);
    return frac_part <= -0.5 ? int_part - 1.0
            : frac_part >= 0.5 ? int_part + 1.0
            : int_part;
}

long double roundl(long double x) {
	long double int_part, frac_part;

    frac_part = modfl(x, &int_part);
    return frac_part <= -0.5 ? int_part - 1.0
            : frac_part >= 0.5 ? int_part + 1.0
            : int_part;
}
