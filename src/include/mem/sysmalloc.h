/**
 * @file
 * @brief Dynamic memory allocator arbitrary size. Terehov style.
 * @details XXX sysmalloc docs
 *
 * @date 02.05.10
 * @author Michail Skorginskii
 */

#ifndef MEM_SYSMALLOC_H_
#define MEM_SYSMALLOC_H_

#include <stddef.h>

/**
 * Dynamic memory allocator arbitrary size.
 *
 * @param size requested memory size
 *
 * @return pointer to the memory of the requested size.
 * @retval 0 if there are no memory
 */
extern void *sysmalloc(size_t size /*, int priority */);

/**
 * Free memory function.
 *
 * @param ptr pointer at the memory, that must be free
 */
extern void sysfree(void *ptr);

extern void *sysmemalign(size_t boundary, size_t size);
extern void *sysrealloc(void *ptr, size_t size);
extern void *syscalloc(size_t nmemb, size_t size);

#endif /* MEM_SYSMALLOC_H_ */
