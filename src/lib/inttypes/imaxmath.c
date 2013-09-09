/**
 * @file
 * @brief
 *
 * @date 23.08.13
 * @author Alexander Kalmuk
 */

#include <inttypes.h>

intmax_t imaxabs(intmax_t j) {
	return (j >= 0 ? j : -j);
}

imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom) {
	imaxdiv_t result;

	result.quot = numer / denom;
	result.rem = numer % denom;

	return result;
}
