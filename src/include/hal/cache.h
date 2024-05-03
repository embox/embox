/**
 * @file
 * @brief
 *
 * @date 12.08.2023
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */
#ifndef HAL_CACHE_H_
#define HAL_CACHE_H_

#include <stddef.h>

extern void dcache_inval(const void *data, size_t size);
extern void dcache_flush(const void *data, size_t size);

#endif /* HAL_CACHE_H_ */
