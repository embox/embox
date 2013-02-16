/**
 * @file
 *
 * @brief Just demonstrates structure of example
 * @details This demo presents "hello world" example
 *
 * @date 16.08.11
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>

#include <util/macro.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

/**
 * Example's execution routing
 * It has been declare with macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	int i;

	printf("Hi, I'm %s\n", MACRO_STRING(__EMBUILD_MOD__));

	printf("My arguments are:\n");
	i = 0;
	do {
		printf("\t[%2d] %s\n", i, argv[i]);
	} while (++i < argc);

	return ENOERR;
}
