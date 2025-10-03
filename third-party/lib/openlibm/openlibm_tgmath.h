/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 03.09.25
 */

#ifndef THIRD_PARTY_LIB_OPENLIBM_TGMATH_H_
#define THIRD_PARTY_LIB_OPENLIBM_TGMATH_H_

#include <complex.h>
#include <math.h>

#define __TYPE_GENERIC_FUNC_1(func, ...) \
	__builtin_tgmath(func##f, func, func##l, __VA_ARGS__)

#define __TYPE_GENERIC_FUNC_2(func1, func2, ...)                           \
	__builtin_tgmath(func1##f, func1, func1##l, func2##f, func2, func2##l, \
	    __VA_ARGS__)

/* math.h */
#define atan2(a, b)      __TYPE_GENERIC_FUNC_1(atan2, a, b)
#define cbrt(a)          __TYPE_GENERIC_FUNC_1(cbrt, a)
#define ceil(a)          __TYPE_GENERIC_FUNC_1(ceil, a)
#define copysign(a, b)   __TYPE_GENERIC_FUNC_1(copysign, a, b)
#define erf(a)           __TYPE_GENERIC_FUNC_1(erf, a)
#define erfc(a)          __TYPE_GENERIC_FUNC_1(erfc, a)
#define exp2(a)          __TYPE_GENERIC_FUNC_1(exp2, a)
#define expm1(a)         __TYPE_GENERIC_FUNC_1(expm1, a)
#define fdim(a, b)       __TYPE_GENERIC_FUNC_1(fdim, a, b)
#define floor(a)         __TYPE_GENERIC_FUNC_1(floor, a)
#define fma(a, b, c)     __TYPE_GENERIC_FUNC_1(fma, a, b, c)
#define fmax(a, b)       __TYPE_GENERIC_FUNC_1(fmax, a, b)
#define fmin(a, b)       __TYPE_GENERIC_FUNC_1(fmin, a, b)
#define fmod(a, b)       __TYPE_GENERIC_FUNC_1(fmod, a, b)
#define frexp(a, b)      __TYPE_GENERIC_FUNC_1(frexp, a, b)
#define hypot(a, b)      __TYPE_GENERIC_FUNC_1(hypot, a, b)
#define ilogb(a)         __TYPE_GENERIC_FUNC_1(ilogb, a)
#define ldexp(a, b)      __TYPE_GENERIC_FUNC_1(ldexp, a, b)
#define lgamma(a)        __TYPE_GENERIC_FUNC_1(lgamma, a)
#define llrint(a)        __TYPE_GENERIC_FUNC_1(llrint, a)
#define llround(a)       __TYPE_GENERIC_FUNC_1(llround, a)
#define log10(a)         __TYPE_GENERIC_FUNC_1(log10, a)
#define log1p(a)         __TYPE_GENERIC_FUNC_1(log1p, a)
#define log2(a)          __TYPE_GENERIC_FUNC_1(log2, a)
#define logb(a)          __TYPE_GENERIC_FUNC_1(logb, a)
#define lrint(a)         __TYPE_GENERIC_FUNC_1(lrint, a)
#define lround(a)        __TYPE_GENERIC_FUNC_1(lround, a)
#define nearbyint(a)     __TYPE_GENERIC_FUNC_1(nearbyint, a)
#define nextafter(a, b)  __TYPE_GENERIC_FUNC_1(nextafter, a, b)
#define nexttoward(a, b) __TYPE_GENERIC_FUNC_1(nexttoward, a, b)
#define remainder(a, b)  __TYPE_GENERIC_FUNC_1(remainder, a, b)
#define remquo(a, b, c)  __TYPE_GENERIC_FUNC_1(remquo, a, b, c)
#define rint(a)          __TYPE_GENERIC_FUNC_1(rint, a)
#define round(a)         __TYPE_GENERIC_FUNC_1(round, a)
#define scalbn(a, b)     __TYPE_GENERIC_FUNC_1(scalbn, a, b)
#define scalbln(a, b)    __TYPE_GENERIC_FUNC_1(scalbln, a, b)
#define tgamma(a)        __TYPE_GENERIC_FUNC_1(tgamma, a)
#define trunc(a)         __TYPE_GENERIC_FUNC_1(trunc, a)
/* complex.h */
#define carg(a)          __TYPE_GENERIC_FUNC_1(carg, a)
#define cimag(a)         __TYPE_GENERIC_FUNC_1(cimag, a)
#define conj(a)          __TYPE_GENERIC_FUNC_1(conj, a)
#define cproj(a)         __TYPE_GENERIC_FUNC_1(cproj, a)
#define creal(a)         __TYPE_GENERIC_FUNC_1(creal, a)
/* math.h & complex.h */
#define acos(a)          __TYPE_GENERIC_FUNC_2(acos, cacos, a)
#define asin(a)          __TYPE_GENERIC_FUNC_2(asin, casin, a)
#define atan(a)          __TYPE_GENERIC_FUNC_2(atan, catan, a)
#define acosh(a)         __TYPE_GENERIC_FUNC_2(acosh, cacosh, a)
#define asinh(a)         __TYPE_GENERIC_FUNC_2(asinh, casinh, a)
#define atanh(a)         __TYPE_GENERIC_FUNC_2(atanh, catanh, a)
#define cos(a)           __TYPE_GENERIC_FUNC_2(cos, ccos, a)
#define sin(a)           __TYPE_GENERIC_FUNC_2(sin, csin, a)
#define tan(a)           __TYPE_GENERIC_FUNC_2(tan, ctan, a)
#define cosh(a)          __TYPE_GENERIC_FUNC_2(cosh, ccosh, a)
#define sinh(a)          __TYPE_GENERIC_FUNC_2(sinh, csinh, a)
#define tanh(a)          __TYPE_GENERIC_FUNC_2(tanh, ctanh, a)
#define exp(a)           __TYPE_GENERIC_FUNC_2(exp, cexp, a)
#define log(a)           __TYPE_GENERIC_FUNC_2(log, clog, a)
#define pow(a, b)        __TYPE_GENERIC_FUNC_2(pow, cpow, a, b)
#define sqrt(a)          __TYPE_GENERIC_FUNC_2(sqrt, csqrt, a)
#define fabs(a)          __TYPE_GENERIC_FUNC_2(fabs, cabs, a)

#endif /* THIRD_PARTY_LIB_OPENLIBM_TGMATH_H_ */
