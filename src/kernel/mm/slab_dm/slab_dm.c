/**
 * @file
 * @brief Dynamic slab allocator
 *
 * @date 14.12.2010
 * @author Dmitry Zubarvich
 */

#include <kernel/printk.h>
#include <lib/list.h>
#include <string.h>
#include <kernel/mm/slab_dm.h>

typedef struct slab {
	struct list_head list;
	struct list_head free_obj_begin;
	unsigned int count_full_obj;
} slab_t;

/**
 * Define count of objects which can be put to one slab
 */
static unsigned int kmem_cache_estimate() {
	return NUM_OF_OBJECTS_IN_SLAB;
}

/**
 * Free memory which occupied by slab
 * @param slab_ptr the pointer to slab which must be deleted
 */
static void slab_destroy(slab_t *slab_ptr) {
}

/**
 * Return pointer to object for which allocate memory
 * @param cache_ptr is pointer to cache which must contain object of this type
 */
static void *kmem_cache_alloc(kmem_cache_t *cache_ptr) {
}

/**
 * Free memory from something object
 * @param cache_ptr is pointer to cache which must contain object of this type
 * @param obj is object which must be deleted
 */
static void kmem_cache_free(kmem_cache_t *cache_ptr, void* obj) {
}

kmem_cache_t *kmem_cache_create(char *name, size_t obj_size) {
	kmem_cache_t *cache_ptr = kmem_cache_alloc(&kmalloc_cache);

	strcpy(cache_ptr->name, name);
	cache_ptr->obj_size = ALIGN_UP(obj_size, sizeof(struct list_head));
	cache_ptr->num = kmem_cache_estimate();
	cache_ptr->slabs_full.next =&(cache_ptr->slabs_full);
	cache_ptr->slabs_full.prev = &(cache_ptr->slabs_full);
	cache_ptr->slabs_partial.next = &(cache_ptr->slabs_partial);
	cache_ptr->slabs_partial.prev = &(cache_ptr->slabs_partial);
	cache_ptr->slabs_free.next = &(cache_ptr->slabs_free);
	cache_ptr->slabs_free.prev = &(cache_ptr->slabs_free);
	list_add(&(cache_ptr->next), &(kmalloc_cache.next));
	return cache_ptr;
}

void kmem_cache_destroy(kmem_cache_t *cache_ptr) {
	struct list_head *ptr;
	list_for_each(ptr, &cache_ptr->slabs_free) {
		slab_destroy(list_entry(ptr, slab_t, list));
	}
	list_for_each(ptr, &cache_ptr->slabs_full) {
			slab_destroy(list_entry(ptr, slab_t, list));
		}
	list_for_each(ptr, &cache_ptr->slabs_partial) {
			slab_destroy(list_entry(ptr, slab_t, list));
		}
	kmem_cache_free(&kmalloc_cache, cache_ptr);
}

void *smalloc(size_t size) {
}

void sfree(void* obj) {
}
