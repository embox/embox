/**
 * @file
 * @brief Inheritance example
 * @date 28.12.12
 * @author Felix Sulima
 */

#include <cstdio>
#include <new>

#include <embox/test.h>

#include "test_cxx.h"

EMBOX_TEST_SUITE_EXT("c++ inheritance test", NULL, NULL, NULL, NULL);

namespace {

class Base {
public:
	Base(int val) {
		test_emit('a');
	}
	virtual void pure_virtual_fn(void) = 0;
	virtual ~Base() = 0;
};

Base::~Base() {
	test_emit('b');
}

class Derived: public Base {
public:
	Derived(int val): Base(val) {
		test_emit('c');
	}
	~Derived() {
		test_emit('d');
	}
	void pure_virtual_fn(void) {
		test_emit('e');
	}
};

TEST_CASE("Inheritance test") {
	{ Derived derived(0); }
	test_assert_emitted("acdb");
}

TEST_CASE("Calling virtual function") {
	{
		Derived derived(0);
		derived.pure_virtual_fn();
	}
	test_assert_emitted("acedb");
}

} /* namespace */
