/**
 *@file
 *@brief slab allocator
 * some beginning in this way
 *
 *@date 27.11.2010
 *@author Alexandr Kalmuk
 *@author Kirill Tyushev
 */

#include <lib/list.h>
#include <string.h>
#include <kernel/mm/slab.h>

/**
 * some caches
 */
static char cache1_body[SLAB_SIZE1];
kmem_cache_t cache1 = { cache1_body, OBJECT_SIZE1, (SLAB_SIZE1 - sizeof(slab_t))
		/ (OBJECT_SIZE1 + sizeof(struct list_head)), "cache1", 0 };

static char cache2_body[SLAB_SIZE2];
kmem_cache_t cache2 = { cache2_body, OBJECT_SIZE2, (SLAB_SIZE2 - sizeof(slab_t))
		/ (OBJECT_SIZE2 + sizeof(struct list_head)), "cache2", 0 };

static char cache3_body[SLAB_SIZE3];
kmem_cache_t cache3 = { cache3_body, OBJECT_SIZE3, (SLAB_SIZE3 - sizeof(slab_t))
		/ (OBJECT_SIZE3 + sizeof(struct list_head)), "cache3", 0 };

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

/**
 * allocate single object from the cache and return it to the caller
 * @param cache corresponding to allocating object
 * @return the address of allocated object
 */
void* kmem_cache_alloc(kmem_cache_t* cachep) {
	slab_t* slab;
	void* objp;

	if (cachep == NULL)
		return NULL;

	slab = (slab_t*)cachep->cache_begin;

	if (!(cachep->hasinit)) {
		kmem_slab_init(slab, cachep);
		cachep->hasinit = 1;
	}

	if (list_empty(&(slab->obj_ptr)))
		return NULL;

	objp = slab->obj_ptr.next;
	list_del(slab->obj_ptr.next);

	return objp + sizeof(struct list_head);
}

/**
 * free an object and return it to the cache
 * @param cachep corresponding to freeing object
 * @param objp which will be free
 */
void  kmem_cache_free(kmem_cache_t* cachep, void* objp) {
	struct list_head* ptr_begin;
	slab_t* slab;

	if (objp == NULL || cachep == NULL)
		return;

	slab = (slab_t*)cachep->cache_begin;

	ptr_begin = (struct list_head*)(objp - sizeof(struct list_head));
	list_add(ptr_begin, &(slab->obj_ptr));
}
