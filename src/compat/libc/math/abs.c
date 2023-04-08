#include <math.h>

double fabs(double x) {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}

float fabsf(float x) {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}

long double fabsl(long double x) {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}
