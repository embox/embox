#include <math.h>

long double logl(long double x) {
	long double multiplier = 1.;
	long double res = 1.;
	long double t = M_E;
	if (x <= 0) {
		return -MAXFLOAT;
	}
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
	return log(x) * log(10.);
}
