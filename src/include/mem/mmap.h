/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#ifndef MEM_VMEM_H_
#define MEM_VMEM_H_

#include <module/embox/mem/mmap_api.h>

extern void mmap_init(struct mmap *mmap);
extern struct marea *mmap_place_marea(struct mmap *mmap, uint32_t start, uint32_t end, uint32_t flags);

#endif
