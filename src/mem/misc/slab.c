/**
 * @file
 * @brief SLAB allocator
 *
 * @date 14.12.10
 * @author Dmitry Zubarevich
 * @author Kirill Tyushev
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <embox/unit.h>
#include <lib/libds/array.h>
#include <lib/libds/dlist.h>
#include <lib/libds/slist.h>
#include <mem/heap.h>
#include <mem/misc/slab.h>
#include <mem/page.h>
#include <mem/phymem.h>
#include <util/binalign.h>

EMBOX_UNIT_INIT(slab_init);

/**
 * slab descriptor
 */
typedef struct slab {
	/* pointer to other slabs in cache */
	struct dlist_head cache_link;
	/* list of free objects*/
	struct slist free_blocks;
	/* num of objects allocated in the slab */
	unsigned int inuse;
} slab_t;

/* some information about page  */
typedef struct page_info {
	cache_t *cache;
	slab_t *slab;
} page_info_t;

static struct page_allocator *slab_pa;

#if 0
#define SLAB_ALLOCATOR_DEBUG
#endif

#define HEAP_SIZE OPTION_MODULE_GET(embox__mem__slab, NUMBER, heap_size)

static char *heap_start_ptr;

static page_info_t pages[HEAP_SIZE / PAGE_SIZE()];

/* macros to finding the cache and slab which an obj belongs to */
#define SET_PAGE_CACHE(pg, x) ((pg)->cache = (x))
#define GET_PAGE_CACHE(pg)    ((pg)->cache)
#define SET_PAGE_SLAB(pg, x)  ((pg)->slab = (x))
#define GET_PAGE_SLAB(pg)     ((pg)->slab)

#ifdef SLAB_ALLOCATOR_DEBUG
void print_slab_info(cache_t *cachep, slab_t *slabp) {
	int free_elems_count = 0;
	struct list_head *elem;

	list_for_each(elem, &slabp->free_blocks) {
		free_elems_count++;
	}
	printf("slabp->inuse: %d\n", slabp->inuse);
	printf("Number of objects allocated in the slab: %d\n\n",
	    cachep->num - free_elems_count);
}
#endif

/* return information about page which an object belongs to */
static page_info_t *ptr_to_page(void *objp) {
	unsigned int index = ((uintptr_t)objp - (uintptr_t)heap_start_ptr)
	                     / PAGE_SIZE();
	return &(pages[index]);
}

/* main cache which will contain another descriptors of caches */
cache_t cache_chain = {
    .name = "__cache_chain",
    .num = (PAGE_SIZE() * CACHE_CHAIN_SIZE - binalign_bound(sizeof(slab_t), 4))
           / binalign_bound(sizeof(cache_t), 4),
    .obj_size = binalign_bound(sizeof(cache_t), sizeof(struct slist_link)),
    .slabs_full = DLIST_INIT(cache_chain.slabs_full),
    .slabs_free = DLIST_INIT(cache_chain.slabs_free),
    .slabs_partial = DLIST_INIT(cache_chain.slabs_partial),
    .next = DLIST_INIT(cache_chain.next),
    .slab_order = CACHE_CHAIN_SIZE,
    .growing = true,
};

/**
 * Free memory which occupied by slab
 * @param slab_ptr the pointer to slab which must be deleted
 */
static void cache_slab_destroy(cache_t *cachep, slab_t *slabp) {
	page_free(slab_pa, slabp, slabp->inuse);
}

/* init slab descriptor and slab objects */
static void cache_slab_init(cache_t *cachep, slab_t *slabp) {
	char *elem = (char *)slabp + binalign_bound(sizeof(slab_t), 4);

	slabp->inuse = 0;
	dlist_head_init(&slabp->cache_link);
	slist_init(&slabp->free_blocks);

	for (int i = 0; i < cachep->num; i++) {
		slist_add_first_link(slist_link_init((struct slist_link *)elem),
		    &slabp->free_blocks);
		elem += cachep->obj_size;
	}
}

/* grow (by 1) the number of slabs within a cache */
static int cache_grow(cache_t *cachep) {
	int pages_count;
	page_info_t *page;
	slab_t *slabp;
	size_t slab_size = 1 << cachep->slab_order;

	if (!(slabp = (slab_t *)page_alloc(slab_pa, slab_size)))
		return 0;

	page = ptr_to_page(slabp);
	pages_count = slab_size;

	do {
		SET_PAGE_CACHE(page, cachep);
		SET_PAGE_SLAB(page, slabp);
		page++;
	} while (--pages_count);

	cache_slab_init(cachep, slabp);

	dlist_add_prev(&slabp->cache_link, &cachep->slabs_free);

	return 1;
}

/**
 * Define some information for given slab and object sizes
 * @param gfporder - slab size in 2^n form
 * @param size - the size of a single cache object
 * @param left_ofter - how many bytes will be wasted in slab
 * @param num - how many objects will fit in the slab
 */
static void cache_estimate(unsigned int gfporder, size_t size,
    size_t *left_over, unsigned int *num) {
	int count;
	size_t wastage = PAGE_SIZE() << gfporder; /* total size being asked for */
	size_t base = 0;

	base = sizeof(slab_t);

	/* calculate the number of objects that will fit inside the slab, including the
	 * base slab_t */
	count = 0;
	while (count * size + binalign_bound(base, 4) <= wastage)
		count++;
	if (count > 0)
		count--;

	/* return number objects that fit, and total space wasted */
	*num = count;
	wastage -= count * size;
	wastage -= binalign_bound(base, 4);
	*left_over = wastage;
}

int cache_init(cache_t *cachep, size_t obj_size, size_t obj_num) {
	size_t left_over;

	assert(cachep != NULL);

	cachep->obj_size = binalign_bound(obj_size, sizeof(struct slist_link));
	cachep->slab_order = 0;

	do {
		cache_estimate(cachep->slab_order, cachep->obj_size, &left_over,
		    &cachep->num);

		if (cachep->slab_order >= MAX_SLAB_ORDER) /* order == 3, 8 pages */
			break;

		if (!cachep->num) { /* we could not fit any objects on slab */
			cachep->slab_order++;
			continue;
		}

		/* We want that wastage was lower than
		 * (1 / MAX_INT_FRAGM_ORDER) * 100% */
		if ((left_over * MAX_INT_FRAGM_ORDER)
		    <= (PAGE_SIZE() << cachep->slab_order))
			break; /* Acceptable internal fragmentation. */

		cachep->slab_order++;
	} while (1);

	if (!cachep->num) {
		return -ENOMEM;
	}

	cachep->growing = true;
	dlist_init(&cachep->slabs_full);
	dlist_init(&cachep->slabs_partial);
	dlist_init(&cachep->slabs_free);
	dlist_head_init(&cachep->next);
	dlist_add_prev(&cachep->next, &(cache_chain.next));

	/* Reserve memory for minimum count of objects (obj_num) */
	while (obj_num >= cachep->num) {
		cache_grow(cachep);
		obj_num -= cachep->num;
	}

	if (obj_num != 0) {
		cache_grow(cachep);
	}

#ifdef SLAB_ALLOCATOR_DEBUG
	printf("\n\nCreating cache with name \"%s\"\n", cachep->name);
	printf("Object size: %d\n", cachep->obj_size);
	printf("Slab order: %d\n", cachep->slab_order);
	printf("Number of objects in slab: %d\n", cachep->num);
	printf("Wastage: %d\n\n", left_over);
#endif

	return 0;
}

cache_t *cache_create(char *name, size_t obj_size, size_t obj_num) {
	cache_t *cachep;

	if (!name || strlen(name) >= __CACHE_NAMELEN - 1 || obj_size <= 0
	    || obj_size >= PAGE_SIZE() << MAX_OBJ_ORDER)
		return NULL;

	if (!(cachep = (cache_t *)cache_alloc(&cache_chain))) {
		return NULL;
	}

	strcpy(cachep->name, name);

	if (0 != cache_init(cachep, obj_size, obj_num)) {
		cache_free(&cache_chain, cachep);
		return NULL;
	}

	return cachep;
}

static void destroy_slabs(cache_t *cachep, struct dlist_head *slabs) {
	slab_t *slabp;

	if (dlist_empty(slabs))
		return;
	/* remove this slab from the list */
	dlist_foreach_entry(slabp, slabs, cache_link) {
		dlist_del(&slabp->cache_link);
		cache_slab_destroy(cachep, slabp);
	}
}

int cache_destroy(cache_t *cachep) {
	assert(cachep);

	destroy_slabs(cachep, &cachep->slabs_free);
	destroy_slabs(cachep, &cachep->slabs_full);
	destroy_slabs(cachep, &cachep->slabs_partial);

	dlist_del(&cachep->next);
	cache_free(&cache_chain, cachep);

	return 0;
}

void *cache_alloc(cache_t *cachep) {
	slab_t *slabp;
	void *objp;

	assert(cachep);

	/* getting slab */
	if (dlist_empty(&cachep->slabs_partial)) {
		if (dlist_empty(&cachep->slabs_free)) {
			if (cachep->growing == false || !cache_grow(cachep)) {
				return NULL;
			}
		}
		slabp = dlist_entry(cachep->slabs_free.next, slab_t, cache_link);
	}
	else {
		slabp = dlist_entry(cachep->slabs_partial.next, slab_t, cache_link);
	}

	objp = (void *)slist_remove_first_link(&slabp->free_blocks);

	slabp->inuse++;
	if (slabp->inuse == cachep->num) {
		dlist_del(&slabp->cache_link);
		dlist_add_prev(&slabp->cache_link, &cachep->slabs_full);
	}
	else if (slabp->inuse == 1) {
		dlist_del(&slabp->cache_link);
		dlist_add_prev(&slabp->cache_link, &cachep->slabs_partial);
	}

#ifdef SLAB_ALLOCATOR_DEBUG
	printf("\n\nSlab info after allocating object:");
	print_slab_info(cachep, slabp);
#endif

	return objp;
}

void cache_free(cache_t *cachep, void *objp) {
	slab_t *slabp;
	page_info_t *page;

	assert(cachep);

	if (objp == NULL)
		return;

	page = ptr_to_page(objp);
	slabp = GET_PAGE_SLAB(page);
	slist_add_first_link(slist_link_init((struct slist_link *)objp),
	    &slabp->free_blocks);
	slabp->inuse--;

	if (slabp->inuse == 0) {
		dlist_del(&slabp->cache_link);
		dlist_add_next(&slabp->cache_link, &cachep->slabs_free);
	}
	else if (slabp->inuse + 1 == cachep->num) {
		dlist_del(&slabp->cache_link);
		dlist_add_next(&slabp->cache_link, &cachep->slabs_partial);
	}

#ifdef SLAB_ALLOCATOR_DEBUG
	printf("\n\nSlab info after freeing object:");
	print_slab_info(cachep, slabp);
#endif
}

int cache_shrink(cache_t *cachep) {
	slab_t *slabp;
	int ret = 0;

	assert(cachep);

	dlist_foreach_entry(slabp, &cachep->slabs_free, cache_link) {
		dlist_del(&slabp->cache_link);
		cache_slab_destroy(cachep, slabp);
		ret++;
	}

	return ret;
}

ARRAY_SPREAD_DEF_TERMINATED(struct cache *, __cache_slab_registry, NULL);

static int slab_init(void) {
	extern struct page_allocator *__heap_pgallocator;
	cache_t *cache;
	int page_cnt;
	int err;

	page_cnt = (HEAP_SIZE / PAGE_SIZE() - 2);

	heap_start_ptr = page_alloc(__heap_pgallocator, page_cnt);

	if (NULL == heap_start_ptr) {
		return -1;
	}

	slab_pa = page_allocator_init(heap_start_ptr, page_cnt * PAGE_SIZE(),
	    PAGE_SIZE());

	heap_start_ptr += PAGE_SIZE();

	array_spread_nullterm_foreach(cache, __cache_slab_registry) {
		if ((err = cache_init(cache, cache->obj_size, cache->num))) {
			return err;
		}
	}

	return 0;
}
