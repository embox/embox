/**
 * @file
 * @brief Power functions
 *
 * @date 10.11.11
 * @author Nikolay Korotkiy
 */

#include <math.h>

extern float __powf(float x, float y);
extern double __pow(double x, double y);
extern long double __powl(long double x, long double y);
#if 0
double pow(double x, double y) {
	return __pow(x, y);
}
#endif
float powf(float x, float y) {
	return __powf(x, y);
}

long double powl(long double x, long double y) {
	return __powl(x,y);
}
