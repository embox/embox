/**
 * @file
 * @brief Dynamic memory management
 * @date 04.07.12
 * @author Ilia Vaprol
 */

#include <new>

const std::nothrow_t std::nothrow = { };

static std::new_handler alloc_failure_handler = 0;

std::new_handler std::set_new_handler(std::new_handler handler) throw() {
	std::new_handler prev_handler = alloc_failure_handler;
	alloc_failure_handler = handler;
	return prev_handler;
}

#ifdef __GNUC__
#if __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#endif
#endif
// Implementation of new and delete operators for single object
void* operator new(std::size_t size)  throw() {
	void *ptr = NULL;

	if ((ptr = std::malloc(size)) == 0) {
		if (alloc_failure_handler) {
			alloc_failure_handler();
		}
	}

	return ptr;
}
#ifdef __GNUC__
#if __GNUC__ > 6
#pragma GCC diagnostic pop
#endif
#endif

void* operator new(std::size_t size, const std::nothrow_t& nothrow_const) throw() {
	void *ptr = NULL;

	if ((ptr = std::malloc(size)) == 0) {
		if (alloc_failure_handler) {
			alloc_failure_handler();
		}
	}

	return ptr;
}

void operator delete(void* ptr) throw() {
	std::free(ptr);
}

#if defined(__cpp_sized_deallocation)
void operator delete(void* ptr, std::size_t) throw() {
	/* XXX */
	std::free(ptr);
}

void operator delete[](void* ptr, std::size_t) throw() {
	/* XXX */
	::operator delete(ptr);
}
#endif

void operator delete(void* ptr, const std::nothrow_t& nothrow_const) throw() {
	::operator delete(ptr);
}

#ifdef __GNUC__
#if __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#endif
#endif
// Forwarding functions for array of objects
void* operator new[](std::size_t size) throw() {
	return ::operator new(size);
}
#ifdef __GNUC__
#if __GNUC__ > 6
#pragma GCC diagnostic pop
#endif
#endif
void* operator new[](std::size_t size, const std::nothrow_t& nothrow_const) throw() {
	return ::operator new(size, nothrow_const);
}

void operator delete[](void* ptr) throw() {
	::operator delete(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& nothrow_const) throw() {
	::operator delete(ptr, nothrow_const);
}
