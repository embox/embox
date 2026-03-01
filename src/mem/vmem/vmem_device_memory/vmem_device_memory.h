/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */

#ifndef MEM_VMEM_DEVICE_MEMORY_H_
#define MEM_VMEM_DEVICE_MEMORY_H_

#include <stddef.h>
#include <stdint.h>

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
extern void *mmap_device_memory(void *addr, size_t len, int prot, int flags,
    uintptr_t physical);

#endif /* MEM_VMEM_DEVICE_MEMORY_H_ */
