/**
 * @file express_tests.h
 * @brief Express tests structure and macroses definition
 * @details Express tests are low level tests for hardware. It can be called during starting system and with user command express.
 * @date Jul 4, 2009
 * @author Anton Bondarev, Alexey Fomin
 */

#ifndef EXPRESS_TESTS_H_
#define EXPRESS_TESTS_H_


typedef void (*info_func_t)();

typedef struct _express_test_descriptor {
	const char *name;
	int (*exec)();
	int execute_on_boot;
	info_func_t info_func;
} express_test_descriptor_t;

#define __semicolon ;
#define DECLARE_EXPRESS_TEST(name, exec, on_boot, info_func) \
	static int exec() __semicolon \
	static const express_test_descriptor_t _descriptor##exec = { name, exec, on_boot, info_func } __semicolon \
	static const express_test_descriptor_t \
		*_pdescriptor##exec __attribute__ ((used, section(".express_tests"))) \
		= &_descriptor##exec
//TODO this function must be static and call from main if express_tests.c was compiled in system
//express test must be low level test
int express_tests_execute();

#endif /* EXPRESS_TESTS_H_ */
