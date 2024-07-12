/**
 * @file
 *
 * @date May 08, 2019
 * @author: Denis Deryugin
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

/**
 * @brief Map specific memory address to access device
 *
 * @param addr Virtual address to where you want to map the object
 * @param len Number of bytes to be mapped
 * @param prot Access flags for region (PROT_EXEC/PROT_NOCACHE/etc)
 * @param flags MAP_FIXED/etc
 * @param physical Physical address of memory that's to be mapped
 *
 * @return Pointer to allocated vmem or NULL if failed
 */
void *mmap_device_memory(void *addr, size_t len, int prot, int flags,
    uintptr_t physical) {
	return (void *)physical;
}
