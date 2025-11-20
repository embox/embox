// third-party/lib/ncnn/ncnn_new_delete.cpp

#include <stddef.h>  // даёт size_t из C-рантайма

// placement new
void* operator new(size_t, void* p) /*noexcept*/ {
    return p;
}

// placement delete (парная к placement new)
void operator delete(void*, void*) /*noexcept*/ {
    // ничего делать не нужно
}

// на всякий случай array-версии
void* operator new[](size_t, void* p) /*noexcept*/ {
    return p;
}

void operator delete[](void*, void*) /*noexcept*/ {
    // тоже пусто
}
