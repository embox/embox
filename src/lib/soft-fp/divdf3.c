#include "softfloat.h"

double __divdf3(double a, double b) {
	float64 *_a, *_b, __c;
	double *a_ = &a, *b_ = &b;
        _a = (float64*)a_;
        _b = (float64*)b_;
        __c = float64_div(*_a, *_b);
        return *((double*)(&__c));
}
