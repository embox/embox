/**
 * @file
 * @brief Test of using try, catch, and throw statements
 * @date 16.07.12
 * @author Ilia Vaprol
 */

#include <exception>

#include <embox/test.h>

#include "test_cxx.h"

EMBOX_TEST_SUITE_EXT("c++ exception test", NULL, NULL, NULL, NULL);

namespace {

class MyException: public std::exception {
public:
	MyException() {
	}
	virtual ~MyException() throw() {
	}
	virtual const char *what() const throw() {
		return "MyException";
	}
};

TEST_CASE("Throw/catch exception") {
	try {
		test_emit('a');
		throw MyException();
	}
	catch (MyException &e) {
		test_emit('b');
	}
	test_assert_emitted("ab");
}

/* This function calls itself 100 times expanding call stack,
 * and then throws exception */
static void test_func_throw(void) {
	static int i = 0;

	if (++i < 100) {
		test_func_throw();
	}
	else {
		throw MyException();
	}
}

static void test_func_catch2(void) {
	try {
		test_emit('a');
		test_func_throw();
	}
	catch (std::exception &e) {
		test_emit('b');
	}
}

static void test_func_catch1(void) {
	try {
		test_func_catch2();
	}
	catch (std::exception &e) {
		test_emit('c');
	}
}

TEST_CASE("Catch exception in outer function") {
	/* Throws exception in the inner funtion, catch in outer function */
	test_func_catch1();
	test_assert_emitted("ab");
}

} /* namespace */
