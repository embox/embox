// third-party/lib/ncnn/ncnn_new_delete.cpp

#include <stddef.h>

// placement new
void* operator new(size_t, void* p) /*noexcept*/ {
    return p;
}

void operator delete(void*, void*) /*noexcept*/ {
    // nothing to do
}

void* operator new[](size_t, void* p) /*noexcept*/ {
    return p;
}

void operator delete[](void*, void*) /*noexcept*/ {
    // nothing to do
}
