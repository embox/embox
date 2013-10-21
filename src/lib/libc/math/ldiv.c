/**
 * @file
 * @brief
 *
 * @date 05.04.2012
 * @author Andrey Gazukin
 */

#include <stdlib.h>

ldiv_t ldiv(long num, long denom)
{
	ldiv_t r;

	/* see div.c for comments */

	r.quot = num / denom;
	r.rem = num % denom;
	if (num >= 0 && r.rem < 0) {
		r.quot++;
		r.rem -= denom;
	}
	return (r);
}
