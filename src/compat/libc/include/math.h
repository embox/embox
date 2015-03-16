/**
 * @file
 * @brief Basic numeric operations
 *
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef MATH_H_
#define MATH_H_

#include <sys/cdefs.h>

#define __BSD_VISIBLE 1

#include <module/third_party/lib/OpenLibm.h>

static inline int finite(double x) {
	return isfinite(x);
}

static inline int finitef(float x) {
	return isfinite(x);
}

static inline int finitel(long double x) {
	return isfinite(x);
}

#endif /* MATH_H_ */
