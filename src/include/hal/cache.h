/*
 * @file
 * @brief
 *
 * @date 12.08.2023
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

#ifndef ARCH_DCACHE_H_
#define ARCH_DCACHE_H_

#ifndef __ASSEMBLER__
#include <stddef.h>

extern void dcache_inval(const void *data, size_t size);
extern void dcache_flush(const void *data, size_t size);
#endif /* __ASSEMBLER__ */

#endif /* ARCH_DCACHE_H_ */
