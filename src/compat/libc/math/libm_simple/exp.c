#include <math.h>

long double expl(long double x) {
	return powl(M_E, x);
}

float expf(float x) {
	return expl(x);
}

double exp(double x) {
	return expl(x);
}
