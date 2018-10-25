
#ifndef MATH_H_
#define MATH_H_

#include <stdint.h>
#include <defines/size_t.h>
#include <defines/wchar_t.h>

#include <openlibm_math.h>
#if 0
double sqrt(double x);
double sin(double x);
double exp(double x);
double cos(double x);
double rint(double x);
double log(double x);
double log10(double x);
double pow(double x, double y);
double atan(double x);
double tan(double x);
double ldexp(double x);
float fabsf(float x);
double acos(double x);
double floor(double x);
double ceil(double x);
float powf(float x, float y);
float fmodf(float x, float y);
double fabs(double x);
double atan2(double x, double y);


 long int lrint(double x);
  long int lrintf(float x);
   long int lrintl(long double x);

    long long int llrint(double x);
     long long int llrintf(float x);
      long long int llrintl(long double x);
void *alloca(size_t size);

#endif
int execvp(const char *file, char *const argv[]);
void *alloca(size_t size);

#endif
