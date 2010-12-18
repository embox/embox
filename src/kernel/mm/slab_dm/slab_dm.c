/**
 * @file
 * @brief Dynamic slab allocator
 *
 * @date 14.12.2010
 * @author Dmitry Zubarvich
 * @author Kirill Tyushev
 */

#include <kernel/printk.h>
#include <lib/list.h>
#include <string.h>
#include <kernel/mm/slab_dm.h>
#include <kernel/mm/mpallocator.h>

/**
 * slab descriptor
 */
typedef struct slab {
	/* pointer to other slabs in cache */
	struct list_head list;
	/* list of free objects*/
	struct list_head obj_ptr;
	/* num of objects allocated in the slab */
	unsigned int inuse;
} slab_t;

/* some information about page  */
typedef struct page_info {
	struct list_head list;
}page_info_t;

extern char _heap_start;
extern char _heap_end;
# define HEAP_START_PTR 	(&_heap_start)
# define HEAP_END_PTR		(&_heap_end)

static page_info_t pages[CONFIG_HEAP_SIZE / CONFIG_PAGE_SIZE];

/* macros to finding the cache and slab which an obj belongs to */
#define SET_PAGE_CACHE(pg, x)  ((pg)->list.next = (struct list_head *)(x))
#define GET_PAGE_CACHE(pg)    ((kmem_cache_t *)(pg)->list.next)
#define SET_PAGE_SLAB(pg, x)   ((pg)->list.prev = (struct list_head *)(x))
#define GET_PAGE_SLAB(pg)     ((slab_t *)(pg)->list.prev)

/**
 * Main cache which will contain another descriptors of caches
 */
static kmem_cache_t kmalloc_cache = {
		.name = "__main_cache",
		.num = NUM_OF_OBJECTS_IN_SLAB,
		.obj_size = ALIGN_UP(sizeof(kmem_cache_t), sizeof(struct list_head)),
		.slabs_full = {NULL, NULL},
		.slabs_free = {NULL, NULL},
		.slabs_partial = {NULL, NULL},
		.next = { &(kmalloc_cache.next), &(kmalloc_cache.next) },
		.growing = false,
		.slab_size = ALIGN_UP((ALIGN_UP(sizeof(slab_t), sizeof(struct list_head)) +
				ALIGN_UP(sizeof(kmem_cache_t), sizeof(struct list_head)) *
				 NUM_OF_OBJECTS_IN_SLAB), CONFIG_PAGE_SIZE) / CONFIG_PAGE_SIZE };

/* return information about page which an object belongs to */
static page_info_t* virt_to_page(void *objp) {
	unsigned int index = ((unsigned int)objp - (unsigned int)HEAP_START_PTR) / CONFIG_PAGE_SIZE;
	return &(pages[index]);
}

/**
 * Free memory which occupied by slab
 * @param slab_ptr the pointer to slab which must be deleted
 */
static void slab_destroy(slab_t *slab_ptr) {

}

/* init slab descriptor and slab objects */
static void init_slab(kmem_cache_t *cachep, slab_t *slabp) {
	char* ptr_begin = (char*)slabp +
			ALIGN_UP(sizeof(slab_t), sizeof(struct list_head));
	struct list_head* elem;

	slabp->inuse = 0;
	slabp->list.next = &slabp->list;
	slabp->list.prev = &slabp->list;
	slabp->obj_ptr.next = &slabp->obj_ptr;
	slabp->obj_ptr.prev = &slabp->obj_ptr;

	for (int i = 0; i < cachep->num; i++) {
		elem = (struct list_head*) (ptr_begin + i * cachep->obj_size);
		list_add(elem, &slabp->obj_ptr);
	}
}

/* grow (by 1) the number of slabs within a cache */
static int kmem_dmcache_grow(kmem_cache_t *cachep) {
	int pages_count;
	page_info_t *page;
	slab_t* slabp;

	if (!(slabp = (slab_t*)mpalloc(cachep->slab_size)))
		return 0;

	page = virt_to_page(slabp);
	pages_count = cachep->slab_size;

	do {
		SET_PAGE_CACHE(page, cachep);
		SET_PAGE_SLAB(page, slabp);
		page++;
	} while (--pages_count);

	init_slab(cachep, slabp);

	cachep->growing = true;
	list_add(&slabp->list, &cachep->slabs_free);

	return 1;
}

/**
 * Define count of objects which can be put to one slab
 */
static unsigned int kmem_dmcache_estimate() {
	return NUM_OF_OBJECTS_IN_SLAB;
}

kmem_cache_t *kmem_dmcache_create(char *name, size_t obj_size) {
	kmem_cache_t *cache_ptr = (kmem_cache_t*)kmem_dmcache_alloc(&kmalloc_cache);

	strcpy(cache_ptr->name, name);
	cache_ptr->obj_size = ALIGN_UP(obj_size, sizeof(struct list_head));
	cache_ptr->num = kmem_dmcache_estimate();
	cache_ptr->slab_size = ALIGN_UP((ALIGN_UP(sizeof(slab_t), sizeof(struct list_head)) +
			       cache_ptr->obj_size * cache_ptr->num), CONFIG_PAGE_SIZE) / CONFIG_PAGE_SIZE;
	cache_ptr->growing = false;
	cache_ptr->slabs_full.next =&(cache_ptr->slabs_full);
	cache_ptr->slabs_full.prev = &(cache_ptr->slabs_full);
	cache_ptr->slabs_partial.next = &(cache_ptr->slabs_partial);
	cache_ptr->slabs_partial.prev = &(cache_ptr->slabs_partial);
	cache_ptr->slabs_free.next = &(cache_ptr->slabs_free);
	cache_ptr->slabs_free.prev = &(cache_ptr->slabs_free);
	list_add((struct list_head*)cache_ptr, &(kmalloc_cache.next));
	return cache_ptr;
}

void kmem_dmcache_destroy(kmem_cache_t *cache_ptr) {
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
	kmem_dmcache_free(&kmalloc_cache, cache_ptr);
}

void *kmem_dmcache_alloc(kmem_cache_t *cachep) {
	slab_t *slabp;
	void *objp;

	/* getting slab */
	if (list_empty(&(cachep->slabs_partial))) {
		if (list_empty(&(cachep->slabs_free)))
			if(!kmem_dmcache_grow(cachep))
				return NULL;
		slabp = (slab_t*)cachep->slabs_free.next;
	} else {
		slabp = (slab_t*)cachep->slabs_partial.next;
	}

	objp = slabp->obj_ptr.next;
	list_del(slabp->obj_ptr.next);

	slabp->inuse++;
	if (slabp->inuse == NUM_OF_OBJECTS_IN_SLAB) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_full);
	} else if (slabp->inuse == 1) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_partial);
	}

	cachep->growing = false;

	return objp;
}

void kmem_dmcache_free(kmem_cache_t *cachep, void* objp) {
	slab_t *slabp;
	page_info_t* page;

	if (objp == NULL)
		return;

	page = virt_to_page(objp);
	slabp = GET_PAGE_SLAB(page);
	list_add((struct list_head*)objp, &slabp->obj_ptr);
	slabp->inuse--;

	if (slabp->inuse == 0) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_free);
	} else if (slabp->inuse + 1 == cachep->num) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_partial);
	}
}

void *smalloc(size_t size) {
}

void sfree(void* obj) {
}
