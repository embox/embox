/**
 * \file sf3.c
 * \date 26.11.09
 * \author sikmir
 */
#include "softfloat.h"

#define PROCESS_OP(op) float32_to_float(float32_##op(float_to_float32(a), float_to_float32(b)))

float __addsf3(float a, float b) {
	return PROCESS_OP(add);
}

float __subsf3(float a, float b) {
	return PROCESS_OP(sub);
}

float __mulsf3(float a, float b) {
	return PROCESS_OP(mul);
}

float __divsf3(float a, float b) {
	return PROCESS_OP(div);
}

int __eqsf2(float a, float b) {
	return float32_eq(float_to_float32(a), float_to_float32(b));
}

float __floatsisf(int i) {
        return float32_to_float(int32_to_float32(i));
}

float __truncdfsf2(double a) {
	return float32_to_float(float64_to_float32(double_to_float64(a)));
}

int __fixsfsi(float a) {
	return float32_to_int32(float_to_float32(a));
}
