/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.04.2014
 */

#include <stdio.h>
#include <unistd.h>

int external_build_lib2_call(void) {
	printf("hello from external lib2\n");

	if (-1 != link("", "")) {
		printf("expecting posix's stub  link to return -1");
		return -1;
	}

	return 314;
}

