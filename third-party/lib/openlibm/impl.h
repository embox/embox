
#ifndef _OPENLIB_FINITE_H
#define _OPENLIB_FINITE_H

#include <sys/cdefs.h>

#define __BSD_VISIBLE 1

#include <../../build/extbld/third_party/lib/OpenLibmImpl/install/openlibm_math.h>

static inline int finite(double x) {
	return isfinite(x);
}

static inline int finitef(float x) {
	return isfinite(x);
}

static inline int finitel(long double x) {
	return isfinite(x);
}

#endif /* _OPENLIB_FINITE_H */
