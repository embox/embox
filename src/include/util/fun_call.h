/**
 * @file
 * @brief
 *
 * @date 03.11.11
 * @author Anton Kozlov
 */

#ifndef UTIL_FUN_CALL_H_
#define UTIL_FUN_CALL_H_

#include <types.h>

#define fun_call_def(def, fun, ...) (fun == NULL ? def : fun(__VA_ARGS__))

#define fun_call(fun, ...) fun_call_def(0, fun, __VA_ARGS__)

#if 0
#define fun_call(fun, ...) \
	do {                              \
		if (NULL != fun) {        \
			fun(__VA_ARGS__); \
		}                         \
	} while (0)
#endif

#define fun_call_res(res, fun, ...) \
	do {                                    \
		if (NULL != fun) {              \
			res = fun(__VA_ARGS__); \
		}                               \
	} while (0)

#endif /* UTIL_FUN_CALL_H_ */
