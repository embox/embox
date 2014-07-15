/**
 * @file
 *
 * @date 02.11.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <math.h>

extern float __sqrtsf2(float a);
extern double __sqrtdf2(double a);
extern long double __sqrttf2(long double a);

double sqrt(double x) {
	return __sqrtdf2(x);
}

float sqrtf(float x) {
	return __sqrtsf2(x);
}

long double sqrtl(long double x) {
	return __sqrttf2(x);
}
