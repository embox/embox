/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.07.24
 */

#ifndef THIRD_PARTY_LIB_OPENLIBM_COMPLEX_H_
#define THIRD_PARTY_LIB_OPENLIBM_COMPLEX_H_

#include <sys/cdefs.h>

#undef complex
#define complex _Complex

#undef _Complex_I
#define _Complex_I 1.0fi

#undef I
#define I _Complex_I

#ifdef __clang__
#define CMPLXF(x, y) ((float complex){x, y})
#define CMPLX(x, y)  ((double complex){x, y})
#define CMPLXL(x, y) ((long double complex){x, y})
#else
#define CMPLXF(x, y) __builtin_complex((float)(x), (float)(y))
#define CMPLX(x, y)  __builtin_complex((double)(x), (double)(y))
#define CMPLXL(x, y) __builtin_complex((long double)(x), (long double)(y))
#endif

__BEGIN_DECLS

double complex cacos(double complex);
double complex casin(double complex);
double complex catan(double complex);
double complex ccos(double complex);
double complex csin(double complex);
double complex ctan(double complex);
double complex cacosh(double complex);
double complex casinh(double complex);
double complex catanh(double complex);
double complex ccosh(double complex);
double complex csinh(double complex);
double complex ctanh(double complex);
double complex cexp(double complex);
double complex clog(double complex);
double cabs(double complex);
double complex cpow(double complex, double complex);
double complex csqrt(double complex);
double carg(double complex);
double cimag(double complex);
double complex conj(double complex);
double complex cproj(double complex);
double creal(double complex);

float complex cacosf(float complex);
float complex casinf(float complex);
float complex catanf(float complex);
float complex ccosf(float complex);
float complex csinf(float complex);
float complex ctanf(float complex);
float complex cacoshf(float complex);
float complex casinhf(float complex);
float complex catanhf(float complex);
float complex ccoshf(float complex);
float complex csinhf(float complex);
float complex ctanhf(float complex);
float complex cexpf(float complex);
float complex clogf(float complex);
float cabsf(float complex);
float complex cpowf(float complex, float complex);
float complex csqrtf(float complex);
float cargf(float complex);
float cimagf(float complex);
float complex conjf(float complex);
float complex cprojf(float complex);
float crealf(float complex);

long double complex cacosl(long double complex);
long double complex casinl(long double complex);
long double complex catanl(long double complex);
long double complex ccosl(long double complex);
long double complex csinl(long double complex);
long double complex ctanl(long double complex);
long double complex cacoshl(long double complex);
long double complex casinhl(long double complex);
long double complex catanhl(long double complex);
long double complex ccoshl(long double complex);
long double complex csinhl(long double complex);
long double complex ctanhl(long double complex);
long double complex cexpl(long double complex);
long double complex clogl(long double complex);
long double cabsl(long double complex);
long double complex cpowl(long double complex, long double complex);
long double complex csqrtl(long double complex);
long double cargl(long double complex);
long double cimagl(long double complex);
long double complex conjl(long double complex);
long double complex cprojl(long double complex);
long double creall(long double complex);

__END_DECLS

#endif /* THIRD_PARTY_LIB_OPENLIBM_COMPLEX_H_ */
