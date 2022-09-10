#include <math.h>

long double floorl(long double x) {
	double tail = x;
	double i;
	while (tail >= 1.) {
		i = 1.0;
		while (i <= tail) {
			i *= 2;
		}
		tail = tail - i / 2;
	}
	return x - tail;
}

double floor(double x) {
	return floorl(x);
}

long double ceill(long double x) {
	return floorl(x + 1);
}

double ceil(double x) {
	return ceill(x);
}
