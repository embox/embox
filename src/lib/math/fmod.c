/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <math.h>

double fmod(double x, double y) {
	double unused;

	return modf(x / y, &unused);
}

float fmodf(float x, float y) {
	float unused;

	return modff(x / y, &unused);
}

long double fmodl(long double x, long double y) {
	long double unused;

	return modfl(x / y, &unused);
}
