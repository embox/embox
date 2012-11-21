/**
 * @file
 *
 * @date Nov 2, 2012
 * @author: Anton Bondarev
 */

extern double __sqrtdf2(double a);

double sqrt(double x) {
	return __sqrtdf2(x);
}
