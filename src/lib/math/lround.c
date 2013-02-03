/**
 * @file
 * @brief
 *
 * @date 03.02.13
 * @author Ilia Vaprol
 */

extern long int __fixsfdi(float a);
extern long int __fixdfdi(double a);
extern long int __fixtfdi(long double a);

long int lround(double x) {
	return __fixdfdi(x);
}

long int lroundf(float x) {
	return __fixsfdi(x);
}

long int lroundl(long double x) {
	return __fixtfdi(x);
}
