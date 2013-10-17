/**
 * @file
 * @brief
 *
 * @date 01.02.13
 * @author fsulima
 */

#include <math.h>

double trunc(double x) {
	return x < 0 ? -floor(-x) : floor(x);
}

float truncf(float x) {
	return x < 0 ? -floorf(-x) : floorf(x);
}

long double truncl(long double x) {
	return x < 0 ? -floorl(-x) : floorl(x);
}


