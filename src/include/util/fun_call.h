/**
 * @file
 * @brief
 *
 * @date 03.11.2011
 * @author Anton Kozlov
 */

#ifndef _UTIL_FUN_CALL_H_
#define _UTIL_FUN_CALL_H_

#include <types.h>

#define fun_call(fun, ...) \
	do {\
		if (NULL != fun) {\
			fun(__VA_ARGS__);\
		}\
	}\
	while (0)

#define fun_call_res(res, fun, ...) \
	do {\
		if (NULL != fun) {\
			res = fun(__VA_ARGS__);\
		}\
	}\
	while (0)

#endif /* _UTIL_FUN_CALL_H_ */
