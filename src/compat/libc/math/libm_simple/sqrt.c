#include <math.h>

static long double sqrt_common(long double x) {
	long double l = 0.;
	long double r = MAXFLOAT;
	long double m = (r + l) / 2.;
	long double eps = 1e-6;
	int steps = 100;

	/* sqrt returns NaN is x < 0 or x is NaN */
	if ((x < 0.) || isnan(x))
		return NAN;
	/* sqrt returns Inf if x is Inf */
	if (isinf(x))
		return INFINITY;
	/* sqrt of 0 = 0 */
	if (x == 0.)
		return 0.;
	
	do {
		if (x > m*m) {
			l = m;
		} else {
			r = m;
		}
		m = r - (r - l) / 2;
	} while (steps-- > 0 && fabs(x - m*m) < eps);

	return m;
}

long double sqrtl(long double x) {
	return sqrt_common(x);
}

float sqrtf(float x) {
	return sqrt_common(x);
}

double sqrt(double x) {
	return sqrt_common(x);
}


