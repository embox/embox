/**
 * @file
 *
 * @brief
 *
 * @date 16.05.2012
 * @author Anton Bondarev
 */
#include <stdlib.h>

lldiv_t lldiv (long long int numer, long long int denom) {
	lldiv_t result;

	result.quot = numer / denom;
	result.rem = numer % denom;
}
