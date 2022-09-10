#include <math.h>

double sqrt(double x) {
	double l = 0.;
	double r = MAXFLOAT;
	double m = (r + l) / 2.;
	double eps = 1e-6;
	int steps = 100;
	if (x <= 0.) {
		return 0.;
	}
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
