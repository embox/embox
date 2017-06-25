/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2014
 */

/* simulate static inline definition included from command-line */
static inline int link(const char *oldpath, const char *newpath) {
	return 1;
}

#include <stdio.h>
#include <unistd.h>

int external_build_lib_call(void) {
	printf("hello from external lib\n");

	if (1 != link("", "")) {
		printf("expecting library stubs' link to return 1");
		return -1;
	}

	return 314;
}

