/**
 * @file express_tests.h
 * @brief Express tests structure and macroses definition
 * @details Express tests are low level tests for hardware. It can be called during starting system and with user command express.
 * @date Jul 4, 2009
 * @author Anton Bondarev, Alexey Fomin
 */

#ifndef EXPRESS_TESTS_H_
#define EXPRESS_TESTS_H_


typedef void (*info_func_t)(void);

typedef struct _express_test_descriptor {
	const char *name;
	const char *short_name;
	int (*exec)(int argc, char** argv);
	int execute_on_boot;
	info_func_t info_func;
} express_test_descriptor_t;

#define DECLARE_EXPRESS_TEST(name, short_name, exec, on_boot, info_func) \
	static int exec(int argc, char** argv); \
	static const express_test_descriptor_t _descriptor##exec = { name, short_name, exec, on_boot, info_func }; \
	static const express_test_descriptor_t \
		*_pdescriptor##exec __attribute__ ((used, section(".express_tests"))) \
		= &_descriptor##exec

#endif /* EXPRESS_TESTS_H_ */
