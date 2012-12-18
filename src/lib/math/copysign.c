/**
 * @file
 * @brief
 *
 * @date 18.12.12
 * @author Ilia Vaprol
 */

#include <math.h>

double copysign(double x, double y) {
    if (((x < 0) && (y > 0)) || ((x > 0) && (y < 0))) {
        return -x;
    }
    return x;
}

float copysignf(float x, float y) {
    if (((x < 0) && (y > 0)) || ((x > 0) && (y < 0))) {
        return -x;
    }
    return x;
}

long double copysignl(long double x, long double y) {
    if (((x < 0) && (y > 0)) || ((x > 0) && (y < 0))) {
        return -x;
    }
    return x;
}
