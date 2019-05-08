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

class MyException : public std::exception
{
public:
	MyException() { }
	virtual ~MyException() throw() { }
	virtual const char* what() const throw() { return "MyException"; }
};

TEST_CASE("Throw/catch exception") {
	try {
		test_emit('a');
		throw MyException();
	}
	catch (MyException& e) {
		test_emit('b');
	}
	test_assert_emitted("ab");
}
} /* namespace */
