#include <math.h>

long double logl(long double x) {
	long double multiplier = 1.;
	long double res = 1.;
	long double t = M_E;
	/*special and non-positive arguments */
	if (x < 0.0) {
		return NAN;
	}
	if (x == 0.0) {
		return -INFINITY;
	}
	if (isinf(x)) {
		return INFINITY;
	}
	if (isnan(x)) {
		return NAN;
	}
	
	/*positive arguments*/
	if (x < 1.0) {
		x = 1 / x;
		multiplier = -1;
	}
	while (t < x) {
		t *= M_E;
		res += 1.;
	}
	return res * multiplier;
}

float logf(float x) {
	return log(x);
}

double log(double x) {
	return logl(x);
}

double log10(double x) {
	return log(x) / log(10.);
}
