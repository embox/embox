/**
 * @file
 *
 * @brief Just demonstrates structure of example
 * @details This demo presents "hello world" example
 *
 * @date 16.08.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>

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

	printf("hello example\n");

	return ENOERR;
}
