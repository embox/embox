/**
 * @file
 * @brief
 *
 * @date 06.01.13
 * @author Ilia Vaprol
 */

#include <lib/math/ieee754.h>

int __nedf2(double x, double y) {
    union ieee754_double *x_ieee, *y_ieee;

    x_ieee = (union ieee754_double *)&x;
    y_ieee = (union ieee754_double *)&y;
	return (x_ieee->ieee.exponent != y_ieee->ieee.exponent)
        || (x_ieee->ieee.mantissa0 != y_ieee->ieee.mantissa0)
        || (x_ieee->ieee.mantissa1 != y_ieee->ieee.mantissa1)
        || ((x_ieee->ieee.negative != y_ieee->ieee.negative)
                && x_ieee->ieee.mantissa0 && x_ieee->ieee.mantissa1);
}
