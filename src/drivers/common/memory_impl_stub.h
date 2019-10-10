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

struct periph_memory_desc {
	uint32_t start;
	uint32_t len;
};

#define PERIPH_MEMORY_DEFINE(_mem_desc, mem_base, mem_len)

#endif /* _DRIVERS_COMMON_MEMORY_H */
