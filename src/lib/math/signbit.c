/**
 * @file
 * @brief
 *
 * @date 18.12.12
 * @author Ilia Vaprol
 */

#include <math.h>

/**
 * TODO it works with mistake when x is -0.0:
 *  right answer is 1, but we return 0
 */

int signbit(double x) {
    return x < 0;
}

int signbitf(float x) {
    return x < 0;
}

int signbitl(long double x) {
    return x < 0;
}
