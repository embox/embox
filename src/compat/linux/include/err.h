/**
 * @file err.h
 * @brief Linux-compatible formatted error messages
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 29.11.2017
 */

#ifndef LINUX_ERR_H_
#define LINUX_ERR_H_

#include <stdlib.h>

/* TODO output format is not the same as "man err" requiers */

static inline void err(int eval, const char *fmt, ...) {
}

static inline void errx(int eval, const char *fmt, ...) {
}

static inline void warn(const char *fmt, ...) {
}

static inline void warnx(const char *fmt, ...) {
}

#endif /* LINUX_ERR_H_ */
