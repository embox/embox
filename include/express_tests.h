/**
 * \file express_tests.h
 * \date Jul 4, 2009
 * \author anton
 * \details
 */

#ifndef EXPRESS_TESTS_H_
#define EXPRESS_TESTS_H_

#include "common.h"

typedef void (*info_func_t)();

typedef struct _express_test_descriptor {
	const char *name;
	int (*exec)();
	BOOL execute_on_boot;
	info_func_t info_func;
} express_test_descriptor_t;

#define __semicolon ;
#define DECLARE_EXPRESS_TEST(name, exec, on_boot, info_func) \
	static int exec() __semicolon \
	static const express_test_descriptor_t _descriptor##exec = { name, exec, on_boot, info_func } __semicolon \
	static const express_test_descriptor_t \
		*_pdescriptor##exec __attribute__ ((used, section(".express_tests"))) \
		= &_descriptor##exec

int express_tests_execute();

#endif /* EXPRESS_TESTS_H_ */
