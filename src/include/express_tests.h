/**
 * @file express_tests.h
 * @brief Express tests structure and macroses definition
 * @details Express tests are low level tests for hardware. It can be called during starting system and with user command express.
 * @date Jul 4, 2009
 * @author Anton Bondarev, Alexey Fomin
 */

#ifndef EXPRESS_TESTS_H_
#define EXPRESS_TESTS_H_


#define EXPRESS_TESTS_PASSED_RETCODE 0
#define EXPRESS_TESTS_FAILED_RETCODE -1
#define EXPRESS_TESTS_INTERRUPTED_RETCODE -2
#define EXPRESS_TESTS_UNABLE_TO_START_RETCODE -3


typedef void (*info_func_t)(void);

typedef struct _express_test_descriptor {
	/*
	 * name ------------ is a short annotation that will be shown on executing test
	 * short_name ------ is a one word string that required to execute test manually
	 * exec ------------ is a pointer to function that implements test
	 * level ----------- determines after which initialization level
	 *                   test should be executed
	 * execute_on_boot - is a flag that indicates whether to launch test
	 *                   on initialization stage or not (only manually)
	 * info_func ------- is a pointer to function that shows complete results
	 *                   of the (previous) test
	 */
	const char *name;
	const char *short_name;
	int (*exec)(int argc, char** argv);
	int level;
	int execute_on_boot;
	info_func_t info_func;
} express_test_descriptor_t;

int express_tests_execute_all( void );
int express_tests_execute( int level );

#define DECLARE_EXPRESS_TEST(name, short_name, exec, level, on_boot, info_func) \
	static int exec(int argc, char** argv); \
	static const express_test_descriptor_t _descriptor##exec = \
                   { name, short_name, exec, level, on_boot, info_func }; \
	static const express_test_descriptor_t \
		*_pdescriptor##exec __attribute__ ((used, section(".express_tests"))) \
		= &_descriptor##exec

#endif /* EXPRESS_TESTS_H_ */
