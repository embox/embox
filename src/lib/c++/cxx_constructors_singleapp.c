/*
 * cxx_constructors_singleapp.c
 *
 *  Created on: 30 janv. 2013
 *      Author: fsulima
 */

#include "cxx_invoke_constructors.h"
#include "cxx_invoke_destructors.h"
#include "cxx_app_startup_termination.h"

void cxx_app_startup(void) {
	cxx_invoke_constructors();
}

void cxx_app_termination(void) {
	cxx_invoke_destructors();
}
