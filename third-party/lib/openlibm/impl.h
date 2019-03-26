
#ifndef _OPENLIB_FINITE_H
#define _OPENLIB_FINITE_H

#include <sys/cdefs.h>

#define __BSD_VISIBLE 1

#include <../../build/extbld/third_party/lib/OpenLibm/install/openlibm_math.h>

static inline int finite(double x) {
	return isfinite(x);
}

static inline int finitef(float x) {
	return isfinite(x);
}

static inline int finitel(long double x) {
	return isfinite(x);
}

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
__BEGIN_DECLS
extern long long int llabs(long long int j);
__END_DECLS
#endif

#endif /* _OPENLIB_FINITE_H */
