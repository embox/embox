/**
 * @file
 * @brief Dynamic memory management
 * @date 04.07.12
 * @author Ilia Vaprol
 */

#include <new>

const std::nothrow_t std::nothrow = { };

static std::new_handler __new_handler = 0;

std::new_handler std::set_new_handler(std::new_handler handler) throw() {
	std::new_handler prev_handler = __new_handler;
	__new_handler = handler;
	return prev_handler;
}

// Implementation of new and delete operators for single object
void* operator new(std::size_t size) throw(std::bad_alloc) {
	void *ptr;

	if (size == 0) { /* std::malloc(0) is not predictable */
		size = 1;
	}

	while ((ptr = std::malloc(size)) == 0) {
		std::new_handler handler = __new_handler;
		if (handler == 0) {
			throw std::bad_alloc();
		}
		handler();
	}

	return ptr;
}

void* operator new(std::size_t size, const std::nothrow_t& nothrow_const) throw() {
	void *ptr;

	if (size == 0) { /* std::malloc(0) is not predictable */
		size = 1;
	}

	while ((ptr = std::malloc(size)) == 0) {
		std::new_handler handler = __new_handler;
		if (handler == 0) {
			return 0;
		}
		try {
			handler();
		}
		catch (const std::bad_alloc&) {
			return 0;
		}
	}

	return ptr;
}

void operator delete(void* ptr) throw() {
	std::free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& nothrow_const) throw() {
	::operator delete(ptr);
}


// Forwarding functions for array of objects
void* operator new[](std::size_t size) throw(std::bad_alloc) {
	return ::operator new(size);
}

void* operator new[](std::size_t size, const std::nothrow_t& nothrow_const) throw() {
	return ::operator new(size, nothrow_const);
}

void operator delete[](void* ptr) throw() {
	::operator delete(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& nothrow_const) throw() {
	::operator delete(ptr, nothrow_const);
}

