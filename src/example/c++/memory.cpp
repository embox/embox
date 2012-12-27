/**
 * @file
 * @brief Example of using new and delete operators
 * @date 12.07.12
 * @author Ilia Vaprol
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

class Base {
private:
	virtual void testPureVirtual(void) = 0;
};

class Hello : private Base {
private:
	void testPureVirtual(void);
public:
	Hello() { std::printf(">> [obj %p] Hello() without any arguments\n", this); }
	explicit Hello(const char* str) { std::printf(">> [obj %p] Hello() with one argument: '%s`\n", this, str); }
	~Hello() { std::printf(">> [obj %p] ~Hello()\n", this); }
};

void Hello::testPureVirtual(void) {
	assert(true);
}

static int run(int argc, char **argv) {
	// stack
	{
		std::puts("Hello without any arguments -- on stack");
		Hello hello;
	}
	{
		std::puts("Hello with one argument -- on stack");
		Hello hello("foo");
	}

	// operator new(size_t, void*)
	{
		std::puts("Hello without any arguments -- via operator new(sz, ptr)");
		char storage[sizeof(Hello)];
		Hello *hello_ptr = new(&storage[0]) Hello;
		hello_ptr->~Hello();
	}
	{
		std::puts("Hello with one argument -- via operator new(sz, ptr)");
		char storage[sizeof(Hello)];
		Hello *hello_ptr = new(&storage[0]) Hello("bar");
		hello_ptr->~Hello();
	}

	// operator new(size_t)
	{
		std::puts("Hello without any arguments -- via operator new(sz)");
		Hello *hello_ptr = new Hello();
		delete hello_ptr;
	}
	{
		std::puts("Hello with one argument -- via operator new(sz)");
		Hello *hello_ptr = new Hello("baz");
		delete hello_ptr;
	}

	// operator new(size_t, const nothrow_t&)
	{
		std::puts("Hello without any arguments -- via operator new(sz, nothrow)");
		Hello *hello_ptr = new(std::nothrow) Hello();
		delete hello_ptr;
	}
	{
		std::puts("Hello with one argument -- via operator new(sz, nothrow)");
		Hello *hello_ptr = new(std::nothrow) Hello("qux");
		delete hello_ptr;
	}

	return 0;
}

