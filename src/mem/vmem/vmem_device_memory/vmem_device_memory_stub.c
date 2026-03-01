/**
 * @file
 *
 * @date May 08, 2019
 * @author: Denis Deryugin
 */

#include <stddef.h>
#include <stdint.h>

#include <mem/vmem_device_memory.h>

void *mmap_device_memory(void *addr, size_t len, int prot, int flags,
    uintptr_t physical) {
	return (void *)physical;
}
