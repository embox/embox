/**
 * @file
 *
 * @date Jun 24, 2013
 * @author: Anton Bondarev
 */

#ifndef UTIL_ERR_H_
#define UTIL_ERR_H_

static inline void *err_ptr(long err_code) {
	return (void *)-err_code;
}

static inline int err(const void *res) {
	return (((long)res) < 0) && (((long)res) > -4096) ? ((long)res) : 0;
}

#endif /* UTIL_ERR_H_ */
