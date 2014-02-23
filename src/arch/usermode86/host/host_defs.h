/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.12.2013
 */

#ifndef __HOST_DEFS_H_
#define __HOST_DEFS_H_

#define _GNU_SOURCE
#include <dlfcn.h>

#define KO ,
#define CONCAT(...) __VA_ARGS__

#define HOST_FNX(_ret, _name, _decl, ...) \
	_ret host_##_name (_decl) { \
		static _ret (*fn_storage)(_decl); \
		if (!fn_storage) { \
			fn_storage = dlsym(RTLD_NEXT, # _name); \
		} \
		return fn_storage(__VA_ARGS__); \
	}

#endif /* __HOST_DEFS_H_ */

