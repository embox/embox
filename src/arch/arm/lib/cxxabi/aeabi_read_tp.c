#include <pthread.h>

void* __aeabi_read_tp() {
    return (void*)pthread_self();
}
