/**
 * @file
 * @brief Dynamic memory management
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <new>

const std::nothrow_t std::nothrow;

// Implementation of new and delete operators for single object
void* operator new(std::size_t size) {
	return std::malloc(size);
}

void* operator new(std::size_t size, const std::nothrow_t& nothrow_const) throw() {
	return 0;
}

void operator delete(void* ptr) throw() {
	std::free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t* nothrow_const) throw() {
	::operator delete(ptr);
}


// Forwarding functions for array of objects
void* operator new[](std::size_t size) {
	return ::operator new(size);
}

void* operator new[](std::size_t size, const std::nothrow_t& nothrow_const) throw() {
	return ::operator new(size, nothrow_const);
}

void operator delete[](void* ptr) throw() {
	::operator delete(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t* nothrow_const) throw() {
	::operator delete(ptr, nothrow_const);
}

