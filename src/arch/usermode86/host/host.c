/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.12.2013
 */

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

HOST_FNX(int, write,
		CONCAT(int fd, const void *buf, int c),
		fd, buf, c)
HOST_FNX(int, putchar, int c, c)
HOST_FNX(int, pause, void)

int main(int argc, char *argv[]) {

	kernel_start();

	return 0;
}
