/**
 * @file
 * @brief
 *
 * @date 12.07.12
 * @author Ilia Vaprol
 */

#include <new>

#include <embox/test.h>

#include "test_cxx.h"

namespace {

int base_ctor;
int base_dtor;

int case_setup(void) {
	base_ctor = base_dtor = 0;
	return 0;
}

} // namespace

EMBOX_TEST_SUITE_EXT("c++ memory test", NULL, NULL, case_setup, NULL);

namespace {

class Base {
protected:
	int field;

public:
	Base() {
		++base_ctor;
	}
	Base(int i): field(i) {
		++base_ctor;
	}
	~Base() {
		++base_dtor;
	}
	int get_field(void) {
		return field;
	}
};

TEST_CASE("Class can allocated on stack") {
	{
		Base base;
		test_assert_equal(base_ctor, 1);
		test_assert_equal(base_dtor, 0);
	}
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 1);
}

TEST_CASE("Class can be allocated on stack using placement new") {
	char storage[sizeof(Base)];
	Base *base_ptr = new (storage) Base();
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 0);
	base_ptr->~Base();
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 1);
}

TEST_CASE("Class can be allocated in heap using nothrow new") {
	Base *base_ptr = new (std::nothrow) Base();
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 0);
	delete base_ptr;
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 1);
}

TEST_CASE("Class can be allocated in heap using throwing new") {
	Base *base_ptr = new Base();
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 0);
	delete base_ptr;
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 1);
}

TEST_CASE("Class can be allocated with class field initialization") {
	Base *base_ptr = new Base(7);
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 0);
	test_assert_equal(base_ptr->get_field(), 7);
	delete base_ptr;
	test_assert_equal(base_ctor, 1);
	test_assert_equal(base_dtor, 1);
}

} // namespace
