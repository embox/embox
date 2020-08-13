/**
 * @file mman.c
 * @brief Stubs for shared memory
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.10.2018
 */

#include <sys/mman.h>
#include <sys/types.h>

int shm_open(const char *name, int oflag, mode_t mode) {
	return -ENOTSUP;
}

int shm_unlink(const char *name) {
	return -ENOTSUP;
}
