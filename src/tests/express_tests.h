/**
 * \file express_tests.h
 * \date Jul 4, 2009
 * \author anton
 * \details
 */
#ifndef EXPRESS_TESTS_H_
#define EXPRESS_TESTS_H_


typedef struct _EXPRESS_TEST_HANDLER{
    char *(*get_test_name)();
    int (*exec)();
}EXPRESS_TEST_HANDLER;


#define REGISTER_EXPRESS_TEST(handler) static void register_express_test(){ \
    __asm__( \
            ".section .express_tests\n\t" \
            ".word %0\n\t" \
            ".text\n" \
            : :"i"(handler)); \
            }


#endif /* EXPRESS_TESTS_H_ */
