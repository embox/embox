#include "softfloat.h"

int __eqdf2(double a, double b) {
	float64 *_a, *_b;
	double *a_ = &a, *b_ = &b;
        _a = (float64*)a_;
        _b = (float64*)b_;
        return float64_eq(*_a, *_b);
}
