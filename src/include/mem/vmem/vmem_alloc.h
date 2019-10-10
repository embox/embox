/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_ALLOC_H_
#define VMEM_ALLOC_H_

#include <stdint.h>

extern uintptr_t *vmem_alloc_table(int lvl);
extern void vmem_free_table(int lvl, uintptr_t *table);

#endif /* VMEM_ALLOC_H_ */
