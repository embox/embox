/**
 * @file
 *
 * @date Jun 24, 2013
 * @author: Anton Bondarev
 */

#ifndef ERR_H_
#define ERR_H_

static inline void *err_ptr(int err_code) {
	return (void *) -err_code;
}

static inline int err(const void *res) {
	return (((int)res) < 0) && (((int)res) > -4096) ? ((int)res) : 0;
}

#endif /* ERR_H_ */
