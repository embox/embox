/**
 * @file
 * @brief
 *
 * @date 01.03.13
 * @author Ilia Vaprol
 */

int __ucmpdi2(unsigned long long a, unsigned long long b) {
	union {
		struct { unsigned long int hi, lo; } s;
		unsigned long long int lli;
	} ua = { .lli = a }, ub = { .lli = b };

	return (ua.s.hi > ub.s.hi) || ((ua.s.hi == ub.s.hi)
				&& (ua.s.lo > ub.s.lo)) ? 2
			: (ua.s.hi == ub.s.hi) && (ua.s.lo == ub.s.lo);
}
