/**
 * @file
 * @brief
 *
 * @date 01.03.13
 * @author Ilia Vaprol
 */

#include <math.h>

#if 0
int finite(double x) {
	return (isinf(x) == 0) && (isnan(x) == 0);
}
#endif

int finitef(float x) {
	return (isinff(x) == 0) && (isnanf(x) == 0);
}

int finitel(long double x) {
	return (isinfl(x) == 0) && (isnanl(x) == 0);
}
