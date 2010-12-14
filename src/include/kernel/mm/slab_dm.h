/**
 * @file
 * @brief Dynamic slab allocator
 *
 * @date 14.12.2010
 * @author Dmitry Zubarvich
 */

#ifndef SLAB_DM_H
#define SLAB_DM_H

/* Length of name of any cache */
#define CHACHE_NEMELEN 16
/* Count of objects which can be contained into the slab */
#define NUM_OF_OBJECTS_IN_SLAB 10
/* Standard align of something */
#define ALIGN_UP(size, align) \
     (((size) + (align) - 1) & (~((align) - 1)))

/**
 * cache descriptor
 */
typedef struct kmem_cache {
	char name[CHACHE_NEMELEN];
	struct list_head slabs_full;
	struct list_head slabs_partial;
	struct list_head slabs_free;
	size_t obj_size;
	/* the number of objects stored on each slab */
	unsigned int num;
	/* pointer to other caches */
	struct list_head next;
} kmem_cache_t;

/**
 * Main cache which will contain another descriptors of caches
 */
static kmem_cache_t kmalloc_cache = {
		.name = "main_cache",
		.num = NUM_OF_OBJECTS_IN_SLAB,
		.obj_size = ALIGN_UP(sizeof(kmem_cache_t), sizeof(struct list_head)),
		.slabs_full = {NULL, NULL},
		.slabs_free = {NULL, NULL},
		.slabs_partial = {NULL, NULL},
		.next = {&(kmalloc_cache.next), &(kmalloc_cache.next)}
};

/**
 * Create of cache
 * @param name of cache
 * @param obj_size is size of contained objects
 */
kmem_cache_t *kmem_cache_create(char *name, size_t obj_size);

/**
 * Destroy of cache
 * @param cache_ptr is pointer to cache which must be deleted
 */
void kmem_cache_destroy(kmem_cache_t *cache_ptr);

/**
 * Return pointer to object for which allocate memory
 * @param size of object
 */
void *smalloc(size_t size);

/**
 * Free memory from something object
 * @param obj is object which must be deleted
 */
void sfree(void* obj);

#endif
