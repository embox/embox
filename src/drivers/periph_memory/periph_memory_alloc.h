/**
 * @file
 *
 * @date Mar 12, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_COMMON_PERIPH_MEMORY_ALLOC_H_
#define SRC_DRIVERS_COMMON_PERIPH_MEMORY_ALLOC_H_

#include <stddef.h>

extern void *periph_memory_alloc(size_t len);
extern void periph_memory_free(void *ptr);

#endif /* SRC_DRIVERS_COMMON_PERIPH_MEMORY_ALLOC_H_ */
