/**
 * \file express_tests.h
 * \date Jul 4, 2009
 * \author anton
 * \details
 */
#ifndef EXPRESS_TESTS_H_
#define EXPRESS_TESTS_H_

typedef struct _express_test_descriptor {
	const char *name;
	int (*exec)();
} express_test_descriptor_t;

#define DECLARE_EXPRESS_TEST(name, exec) \
    static int exec(); \
    static const express_test_descriptor_t _descriptor##exec = { name, exec }; \
    static const express_test_descriptor_t *_pdescriptor##exec \
		__attribute__ ((used, section(".express_tests"))) \
		= &_descriptor##exec;

int express_tests_execute();

#endif /* EXPRESS_TESTS_H_ */
