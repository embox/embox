/**
 * @file
 * @brief Run built-in C++ application
 *
 * @date 21.01.13
 * @author Felix Sulima
 */

#include <embox/cmd.h>

#include <cxxapp.hpp>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	return main(argc,argv);
}
