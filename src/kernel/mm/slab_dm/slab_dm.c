/**
 * @file
 * @brief Dynamic slab allocator
 *
 * @date 14.12.2010
 * @author Dmitry Zubarvich
 * @author Kirill Tyushev
 * @author Alexandr Kalmuk
 */

#include <stdio.h>
#include <kernel/printk.h>
#include <stdlib.h>
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
} page_info_t;

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

/* return information about page which an object belongs to */
static page_info_t* virt_to_page(void *objp) {
	unsigned int index = ((unsigned int) objp - (unsigned int) HEAP_START_PTR)
			/ CONFIG_PAGE_SIZE;
	return &(pages[index]);
}

/**
 * Main cache which will contain another descriptors of caches
 */
static kmem_cache_t kmalloc_cache = {
				.name = "__main_cache",
				.num = NUM_OF_OBJECTS_IN_SLAB,
				.obj_size = ALIGN_UP(sizeof(kmem_cache_t), sizeof(struct list_head)),
				.slabs_full = { &kmalloc_cache.slabs_full, &kmalloc_cache.slabs_full },
				.slabs_free = { &kmalloc_cache.slabs_free, &kmalloc_cache.slabs_free },
				.slabs_partial = { &kmalloc_cache.slabs_partial, &kmalloc_cache.slabs_partial },
				.next = { &kmalloc_cache.next, &kmalloc_cache.next },
				.growing = false,
				.slab_size = ALIGN_UP((ALIGN_UP(sizeof(slab_t), sizeof(struct list_head)) +
								ALIGN_UP(sizeof(kmem_cache_t), sizeof(struct list_head)) *
							     NUM_OF_OBJECTS_IN_SLAB), CONFIG_PAGE_SIZE) / CONFIG_PAGE_SIZE };

/**
 * Free memory which occupied by slab
 * @param slab_ptr the pointer to slab which must be deleted
 */
static void slab_destroy(kmem_cache_t *cachep, slab_t *slabp) {
	page_info_t* page = page = virt_to_page(slabp);
	size_t pages_count = cachep->slab_size;

	do {
		SET_PAGE_CACHE(page, NULL);
		SET_PAGE_SLAB(page, NULL);
		page++;
	} while (--pages_count);

	mpfree(slabp);
}

/* init slab descriptor and slab objects */
static void init_slab(kmem_cache_t *cachep, slab_t *slabp) {
	char* ptr_begin = (char*) slabp
			+ ALIGN_UP(sizeof(slab_t), sizeof(struct list_head));
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

	if (!(slabp = (slab_t*) mpalloc(cachep->slab_size)))
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
	kmem_cache_t *cache_ptr =
			(kmem_cache_t*) kmem_dmcache_alloc(&kmalloc_cache);

	strcpy(cache_ptr->name, name);
	cache_ptr->obj_size = ALIGN_UP(obj_size, sizeof(struct list_head));
	cache_ptr->num = kmem_dmcache_estimate();
	cache_ptr->slab_size
			= ALIGN_UP((ALIGN_UP(sizeof(slab_t), sizeof(struct list_head)) +
							cache_ptr->obj_size * cache_ptr->num), CONFIG_PAGE_SIZE)
					/ CONFIG_PAGE_SIZE;
	cache_ptr->growing = false;
	cache_ptr->slabs_full.next = &(cache_ptr->slabs_full);
	cache_ptr->slabs_full.prev = &(cache_ptr->slabs_full);
	cache_ptr->slabs_partial.next = &(cache_ptr->slabs_partial);
	cache_ptr->slabs_partial.prev = &(cache_ptr->slabs_partial);
	cache_ptr->slabs_free.next = &(cache_ptr->slabs_free);
	cache_ptr->slabs_free.prev = &(cache_ptr->slabs_free);
	list_add((struct list_head*) cache_ptr, &(kmalloc_cache.next));
	return cache_ptr;
}

void kmem_dmcache_destroy(kmem_cache_t *cachep) {
	struct list_head *ptr;
	slab_t *slabp;

	while(1) {
		ptr = cachep->slabs_free.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_free)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		slab_destroy(cachep, slabp);
	}

	while(1) {
		ptr = cachep->slabs_full.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_full)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		slab_destroy(cachep, slabp);
	}

	while(1) {
		ptr = cachep->slabs_partial.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_partial)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		slab_destroy(cachep, slabp);
	}

	kmem_dmcache_free(&kmalloc_cache, cachep);

}

void *kmem_dmcache_alloc(kmem_cache_t *cachep) {
	slab_t *slabp;
	void *objp;

	/* getting slab */
	if (list_empty(&cachep->slabs_partial)) {
		if (list_empty(&cachep->slabs_free))
			if (!kmem_dmcache_grow(cachep))
				return NULL;
		slabp = (slab_t*) cachep->slabs_free.next;
	} else {
		slabp = (slab_t*) cachep->slabs_partial.next;
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
	list_add((struct list_head*) objp, &slabp->obj_ptr);
	slabp->inuse--;

	if (slabp->inuse == 0) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_free);
	} else if (slabp->inuse + 1 == cachep->num) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_partial);
	}
}

int kmem_dmcache_shrink(kmem_cache_t *cachep) {
	slab_t *slabp;
	struct list_head *p;
	int ret = 0;

	while (!cachep->growing) {
		p = cachep->slabs_free.prev;
		/*if list is empty*/
		if (p == &cachep->slabs_free)
			break;
		/* remove this slab from the list */
		slabp = list_entry(cachep->slabs_free.prev, slab_t, list);
		list_del(&slabp->list);
		slab_destroy(cachep, slabp);
		ret++;
	}

	return ret;
}

/**
 * Search fit to this obj_size cache
 * @param obj_size is size for which is searching cache
 * @return pointer to this cache
 */
static kmem_cache_t *find_fit_cache(size_t obj_size) {
	struct list_head *tmp;
	kmem_cache_t *cachep;
	/*pointer to main cache*/
	kmem_cache_t *kmalloc_cachep;

	kmalloc_cachep = &kmalloc_cache;
	tmp = &kmalloc_cachep->next;
	do {
		cachep = (kmem_cache_t*) tmp;
		if (obj_size == cachep->obj_size) {
			return cachep;
		}
		tmp = &cachep->next;
	} while (tmp != &kmalloc_cachep->next);

	return NULL;
}

void *smalloc(size_t size) {
	kmem_cache_t *cachep;
	void *obj_ptr;
	char name[CHACHE_NEMELEN];

	/*different caches must be initialized with different names "__size"*/
	sprintf(name, "%s\n", "__");
	sprintf(name + 2, "%d\n", size);
	cachep = find_fit_cache(size);
	if (cachep != NULL) {
		obj_ptr = kmem_dmcache_alloc(cachep);
		return obj_ptr;
	}
	/*if needed cache is not exist*/
	cachep = kmem_dmcache_create(name, size);
	obj_ptr = kmem_dmcache_alloc(cachep);

	return obj_ptr;
}

/**
 * Converts obj to pointer to appropriate cache
 * obj |-> cache
 * @param obj is pointer to object
 */
static void *virt_to_cache(void *obj) {
	struct page_info *page;
	void *cachep;
	page = virt_to_page(obj);
	cachep = GET_PAGE_CACHE(page);
	return cachep;
}

void sfree(void *obj) {
	kmem_cache_t *cachep;
	cachep = (kmem_cache_t*) virt_to_cache(obj);
	kmem_dmcache_free(cachep, obj);
}
