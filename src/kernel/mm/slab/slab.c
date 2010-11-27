/**
 *@file
 *@brief slab allocator
 * some beginning in this way
 *
 *@date 27.11.2010
 *@author Alexandr Kalmuk
 */

#include <lib/list.h>
#include <string.h>
#include <kernel/mm/slab.h>

/**
 * some caches
 */
static char cache1_body[SLAB_SIZE1];
kmem_cache_t cache1 = { cache1_body, OBJECT_SIZE1, (SLAB_SIZE1 - sizeof(slab_t))
		/ OBJECT_SIZE1, "cache1" };

static char cache2_body[SLAB_SIZE2];
kmem_cache_t cache2 = { cache2_body, OBJECT_SIZE2, (SLAB_SIZE2 - sizeof(slab_t))
		/ OBJECT_SIZE2, "cache2" };

static char cache3_body[SLAB_SIZE3];
kmem_cache_t cache3 = { cache3_body, OBJECT_SIZE3, (SLAB_SIZE3 - sizeof(slab_t))
		/ OBJECT_SIZE3, "cache3" };

/**
 * return the cache descriptor
 * @param cache_name of returning cache
 */
kmem_cache_t* get_cache(char* cache_name) {
	if (cache_name == NULL)
		return NULL;

	if (strcmp(cache_name, cache1.name))
		return &cache1;
	if (strcmp(cache_name, cache2.name))
		return &cache2;
	if (strcmp(cache_name, cache3.name))
		return &cache3;

	return NULL;
}

