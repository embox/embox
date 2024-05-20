#include <pthread.h>

void* __aeabi_read_tp() {
    // Используем pthread_self для получения указателя на текущий поток
    return (void*)pthread_self();
}
