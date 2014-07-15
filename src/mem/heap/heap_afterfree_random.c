/**
 * @file
 * @brief
 *
 * @date 14.02.2014
 * @author Vita Loginova
 */

#include <mem/heap_afterfree.h>

#include <string.h>
#include <stdlib.h>

void afterfree(void *ptr, size_t size) {
	memset(ptr, random(), size);
}
