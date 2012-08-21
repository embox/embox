/**
 * @file
 * @brief Exception Handling routines
 * @date 13.07.12
 * @author Ilia Vaprol
 */

#include <exception>
#include <typeinfo>
#include <cstdlib>

extern "C" void* __cxa_allocate_exception(std::size_t size) throw() {
	void *obj;

	obj = std::malloc(size);
	if (obj == NULL) {
		// no memory.. it's unacceptable
		std::terminate();
	}

	return obj;
}

extern "C" void __cxa_throw(void *, std::type_info *, void (*)(void *)) {
	// TODO
}

extern "C" void __cxa_call_unexpected(void*) {
	// TODO
	std::terminate();
	while(1); // hack, cos it's [noreturn] function
}

extern "C" void* __cxa_begin_catch(void *) throw() {
	// TODO
	return NULL;
}

extern "C" void __cxa_end_catch() {
	// TODO
}

