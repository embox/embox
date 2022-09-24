#include <math.h>

double log(double x) {
	double multiplier = 1.;
	double res = 1.;
	double t = M_E;
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

double log10(double x) {
	return __builtin_log(x) * __builtin_log(10.);
}
