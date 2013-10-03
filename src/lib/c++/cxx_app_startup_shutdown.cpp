/*
 * cxx_app_startup_shutdown.c
 *
 *  Created on: 30 janv. 2013
 *      Author: fsulima
 */

#include "cxx_app_start.h"
#include "cxx_app_startup_termination.h"
#include "cxxapp.hpp"

#if __EXCEPTIONS==0
extern "C" void __do_global_ctors (void) {
}
#endif

int cxx_app_start(int argc, char **argv) {
	int ret;

	cxx_app_startup();
	ret = main(argc,argv);
	cxx_app_termination();

	return ret;
}
