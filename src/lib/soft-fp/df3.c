/**
 * \file df3.c
 * \date 25.11.09
 * \author sikmir
 */
#include "softfloat.h"

#define PROCESS_OP(op) float64_to_double(float64_##op(double_to_float64(a), double_to_float64(b)))

double __adddf3(double a, double b) {
	return PROCESS_OP(add);
}

double __subdf3(double a, double b) {
	return PROCESS_OP(sub);
}

double __muldf3(double a, double b) {
	return PROCESS_OP(mul);
}

double __divdf3(double a, double b) {
	return PROCESS_OP(div);
}

int __eqdf2(double a, double b) {
        return float64_eq(double_to_float64(a), double_to_float64(b));
}

double __floatsidf(int i) {
        return float64_to_double(int32_to_float64(i));
}
