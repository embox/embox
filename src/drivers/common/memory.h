/**
 * @file memory.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2016-08-11
 */

#ifndef _DRIVERS_COMMON_MEMORY_H
#define _DRIVERS_COMMON_MEMORY_H

#include <stdint.h>
#include <lib/libds/array.h>

struct periph_memory_desc {
	uintptr_t start;
	uintptr_t len;
};

#define PERIPH_MAX_SEGMENTS 64

#define PERIPH_MEMORY_DEFINE(dev_name, mem_base, mem_len) \
		static const struct periph_memory_desc dev_name ## _mem = { \
				.start = mem_base, .len = mem_len}; \
	ARRAY_SPREAD_DECLARE(const struct periph_memory_desc *, \
			__periph_mem_registry);	\
	ARRAY_SPREAD_ADD(__periph_mem_registry, \
			&dev_name ## _mem)

extern int periph_desc(struct periph_memory_desc **buff);
extern void *periph_memory_alloc(size_t len);
extern void periph_memory_free(void *ptr);

#endif /* _DRIVERS_COMMON_MEMORY_H */
