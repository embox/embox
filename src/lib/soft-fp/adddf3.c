#include "softfloat.h"

double __adddf3(double a, double b) {
	float64 *_a, *_b, __c;
	double *a_ = &a, *b_ = &b, *c_;
        _a = (float64*)a_;
        _b = (float64*)b_;
        __c = float64_add(*_a, *_b);
        return *((double*)(&__c));
}
