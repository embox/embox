/**
 * @file
 *
 * @date 25.11.09
 * @author Nikolay Korotky
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

int __fixdfsi(double a) {
        return float64_to_int32(double_to_float64(a));
}

double __negdf2(double a) {
	return __subdf3(0.0, a);
}

double __sqrtdf2(double a) {
        return float64_to_double(float64_sqrt(double_to_float64(a)));
}

int __ledf2(double a, double b) {
	return float64_le(double_to_float64(a), double_to_float64(b));
}

int __gedf2(double a, double b) {
	return !float64_lt(double_to_float64(a), double_to_float64(b));
}

int __ltdf2(double a, double b) {
        return float64_lt(double_to_float64(a), double_to_float64(b));
}

int __gtdf2(double a, double b) {
        return !float64_le(double_to_float64(a), double_to_float64(b));
}
