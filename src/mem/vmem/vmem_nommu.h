/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_NOMMU_H
#define VMEM_NOMMU_H

#include <hal/mmu.h>

#define vmem_create_context(ctx)

#define vmem_current_context() 0

#define vmem_free_context(ctx)

#define vmem_translate(a, b, c) (b)

#endif /* VMEM_NOMMU_H */
