/**
 * @file
 * @brief built-in version for some math functions
 *        https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.01.2018
 */

double modf(double x, double *i_ptr) {
	return __builtin_modf(x, i_ptr);
}

double fmod(double x, double y) {
	return __builtin_fmod(x, y);
}

int signbit(double x) {
	return __builtin_signbit(x);
}

double fabs(double x) {
	return __builtin_fabs(x);
}

double roundl(double x) {
	return __builtin_roundl(x);
}

double pow(double x, double y) {
	return __builtin_pow(x,y);
}

double log10(double x) {
	return __builtin_log10(x);
}

long double ceill(long double x) {
	return __builtin_ceill(x);
}

double ceil(double x) {
	return __builtin_ceil(x);
}

float ceilf(float x) {
	return __builtin_ceilf(x);
}

double floor(double x) {
	return __builtin_floor(x);
}

float floorf(float x) {
	return __builtin_floorf(x);
}

long double floorl(long double x) {
	return __builtin_floorl(x);
}
