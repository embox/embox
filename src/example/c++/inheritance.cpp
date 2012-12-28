/**
 * @file
 * @brief Inheritance example
 * @date 28.12.12
 * @author Felix Sulima
 */

#include <framework/example/self.h>

#include <new>
#include <cstdio>

// TEST: include all C++ headers
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <csetjmp>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>
#include <typeinfo>
// END TEST

EMBOX_EXAMPLE(run);

class Base;
typedef void (* pbase_fn_t) (Base &base);

class Base {
public:
	Base(pbase_fn_t fn) {
		std::printf(">> [obj %p] Base(%p)\n", this, fn);
		if (fn) {
			fn(*this);
		}
	}
	virtual void testPureVirtual(void) = 0;
	virtual ~Base() = 0;
};

Base::~Base() {
	std::printf(">> [obj %p] ~Base()\n", this);
}

class Derived : public Base {
public:
	virtual void testPureVirtual(void) { std::printf(">> [obj %p] Derived.testPureVirtual() without any arguments\n", this); }
	Derived(pbase_fn_t fn) : Base(fn) { std::printf(">> [obj %p] Derived(%p)\n", this, fn); }
	~Derived() { std::printf(">> [obj %p] ~Derived()\n", this); }
};

static void callPureVirtual(Base& base) {
	assert(true);
	base.testPureVirtual();
}

static int run(int argc, char **argv) {


	// stack
	{
		std::puts("Derived without any arguments -- on stack");
		Derived derived(NULL);
	}

	// operator new(size_t, void*)
	{
		std::puts("Derived without any arguments -- via operator new(sz, ptr)");
		char storage[sizeof(Derived)];
		Derived *derived_ptr = new(&storage[0]) Derived(NULL);
		derived_ptr->~Derived();
	}

	// operator new(size_t)
	{
		std::puts("Derived without any arguments -- via operator new(sz)");
		Derived *derived_ptr = new Derived(NULL);
		delete derived_ptr;
	}

	// operator new(size_t, const nothrow_t&)
	{
		std::puts("Derived without any arguments -- via operator new(sz, nothrow)");
		Derived *derived_ptr = new(std::nothrow) Derived(NULL);
		delete derived_ptr;
	}

	// Calling pure virtual
	{
		std::puts("Calling pure virtual function");
		Derived derived(callPureVirtual);
	}

	return 0;
}

