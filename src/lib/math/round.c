/**
 * @file
 * @brief
 *
 * @date 07.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

extern float __roundsf2(float a);
extern double __rounddf2(double a);
extern long double __roundtf2(long double a);

double round(double x) {
	return __rounddf2(x);
}

float roundf(float x) {
	return __roundsf2(x);
}

long double roundl(long double x) {
	return __roundtf2(x);
}
