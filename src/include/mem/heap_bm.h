/**
 * @file
 * @brief Boundary markers memory allocation algorithm
 *
 * @date 04.02.14
 * @author Alexander Kalmuk
 */

#ifndef MEM_HEAP_BM_H_
#define MEM_HEAP_BM_H_

#include <sys/types.h>

extern void bm_init(void *heap, size_t size);
extern void *bm_memalign(void *heap, size_t boundary, size_t size);
extern void bm_free(void *heap, void *ptr);
extern int bm_heap_is_empty(void *heap);

#endif /* MEM_HEAP_BM_H_ */
