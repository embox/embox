/**
 * @file
 * @brief Dynamic memory management
 * @date 04.06.12
 * @author Ilia Vaprol
 */

#include <new>

namespace std {
	extern "C" void * malloc(std::size_t);
	extern "C" void free(void *);
}

void * operator new(std::size_t sz) {
	return std::malloc(sz);
}

void * operator new[](std::size_t sz) {
	return ::operator new(sz);
}

void operator delete(void* ptr) {
	std::free(ptr);
}

void operator delete[](void* ptr) {
	return ::operator delete(ptr);
}

