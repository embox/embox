/**
 * @file
 * @brief Run built-in C++ application
 *
 * @date 21.01.13
 * @author Felix Sulima
 */

#include <embox/cmd.h>

#include <cxx_app_start.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	return cxx_app_start(argc,argv);
}
