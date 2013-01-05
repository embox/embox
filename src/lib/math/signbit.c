/**
 * @file
 * @brief
 *
 * @date 18.12.12
 * @author Ilia Vaprol
 */

#include <lib/math/ieee754.h>
#include <math.h>

int signbit(double x) {
    union ieee754_double ieee_x;
    ieee_x.val = x;
    return ieee_x.ieee.negative;
}

int signbitf(float x) {
    union ieee754_single ieee_x;
    ieee_x.val = x;
    return ieee_x.ieee.negative;
}

int signbitl(long double x) {
    union ieee754_double ieee_x;
    ieee_x.val = x; /* FIXME only if size of long double is 64bit */
    return ieee_x.ieee.negative;
}
