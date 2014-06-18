/**
 * @file
 * @brief
 *
 * @date 03.02.13
 * @author Ilia Vaprol
 */

extern long long int __fixsfti(float a);
extern long long int __fixdfti(double a);
extern long long int __fixtfti(long double a);

long long int llround(double x) {
	return __fixdfti(x);
}

long long int llroundf(float x) {
	return __fixsfti(x);
}

long long int llroundl(long double x) {
	return __fixtfti(x);
}
