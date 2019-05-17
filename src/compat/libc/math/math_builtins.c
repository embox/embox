/**
 * @file
 * @brief built-in version for some math functions
 *        https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.01.2018
 */
#include <math.h>

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

double round(double x) {
	return __builtin_round(x);
}
float roundf(float x) {
	return __builtin_roundf(x);
}
long double roundl(long double x) {
	return __builtin_roundl(x);
}

double pow(double x, double y) {
	return __builtin_pow(x,y);
}

float powf(float x, float y) {
	return __builtin_powf(x, y);
}

long double powl(long double x, long double y) {
	return __builtin_powl(x, y);
}

double log10(double x) {
	return __builtin_log10(x);
}

double ceil(double x) {
	return __builtin_ceil(x);
}
float ceilf(float x) {
	return __builtin_ceilf(x);
}
long double ceill(long double x) {
	return __builtin_ceill(x);
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

double acos(double x) {
	return __builtin_acos(x);
}
float acosf(float x) {
	return __builtin_acosf(x);
}
long double acosl(long double x) {
	return __builtin_acosl(x);
}

double asin(double x) {
	return __builtin_asin(x);
}
float asinf(float x) {
	return __builtin_asinf(x);
}
long double asinl(long double x) {
	return __builtin_asinl(x);
}
double atan(double x) {
	return __builtin_atan(x);
}
float atanf(float x) {
	return __builtin_atanf(x);
}
long double atanl(long double x) {
	return __builtin_atanl(x);
}

double atan2(double y, double x) {
	return __builtin_atan2(y, x);
}
float atan2f(float y, float x) {
	return __builtin_atan2f(y, x);
}
long double atan2l(long double y, long double x) {
	return __builtin_atan2l(y, x);
}

double cos(double x) {
	return __builtin_cos(x);
}
float cosf(float x) {
	return __builtin_cosf(x);
}
long double cosl(long double x) {
	return __builtin_cosl(x);
}

double cosh(double x) {
	return __builtin_cosh(x);
}
float coshf(float x) {
	return __builtin_coshf(x);
}
long double coshl(long double x) {
	return __builtin_coshl(x);
}

double exp(double x) {
	return __builtin_exp(x);
}
float expf(float x) {
	return __builtin_expf(x);
}
long double expl(long double x) {
	return __builtin_expl(x);
}

double frexp(double num, int *exp) {
	return __builtin_frexp(num, exp);
}
float frexpf(float num, int *exp) {
	return __builtin_frexpf(num, exp);
}
long double frexpl(long double num, int *exp) {
	return __builtin_frexpl(num, exp);
}

double ldexp(double x, int exp) {
	return __builtin_ldexp(x, exp);
}
float ldexpf(float x, int exp) {
	return __builtin_ldexpf(x, exp);
}
long double ldexpl(long double x, int exp) {
	return __builtin_ldexpl(x, exp);
}

double log(double x) {
	return __builtin_log(x);
}
float logf(float x) {
	return __builtin_logf(x);
}
long double logl(long double x) {
	return __builtin_logl(x);
}

double sin(double x) {
	return __builtin_sin(x);
}
float sinf(float x) {
	return __builtin_sinf(x);
}
long double sinl(long double x) {
	return __builtin_sinl(x);
}

double sinh(double x) {
	return __builtin_sinh(x);
}
float sinhf(float x) {
	return __builtin_sinhf(x);
}
long double sinhl(long double x) {
	return __builtin_sinhl(x);
}

double sqrt(double x) {
	return __builtin_sqrt(x);
}
float sqrtf(float x) {
	return __builtin_sqrtf(x);
}
long double sqrtl(long double x) {
	return __builtin_sqrtl(x);
}

double tan(double x) {
	return __builtin_tan(x);
}
float tanf(float x) {
	return __builtin_tanf(x);
}
long double tanl(long double x) {
	return __builtin_tanl(x);
}

double tanh(double x) {
	return __builtin_tanh(x);
}
float tanhf(float x) {
	return __builtin_tanhf(x);
}
long double tanhl(long double x) {
	return __builtin_tanhl(x);
}
