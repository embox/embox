/**
 * @file
 * @brief
 *
 * @date 05.04.2012
 * @author Andrey Gazukin
 */

#include <stdlib.h>

div_t div(int num, int denom)
{
	div_t r;

	r.quot = num / denom;
	r.rem = num % denom;
	/**
	 * The ANSI standard says that |r.quot| <= |n/d|, where
     * n/d is to be computed in infinite precision.  In other
     * words, we should always truncate the quotient towards
	 * 0, never -infinity.
	 *
	 * Machine division and remainer may work either way when
	 * one or both of n or d is negative.  If only one is
	 * negative and r.quot has been truncated towards -inf,
	 * r.rem will have the same sign as denom and the opposite
	 * sign of num; if both are negative and r.quot has been
	 * truncated towards -inf, r.rem will be positive (will
	 * have the opposite sign of num).  These are considered
	 * `wrong'.
	 *
	 * If both are num and denom are positive, r will always
	 * be positive.
	 *
	 * This all boils down to:
	 *      if num >= 0, but r.rem < 0, we got the wrong answer.
	 * In that case, to get the right answer, add 1 to r.quot and
	 * subtract denom from r.rem.
	 * */

	if (num >= 0 && r.rem < 0) {
		r.quot++;
		r.rem -= denom;
	}
	return (r);
}
