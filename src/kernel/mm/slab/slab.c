/**
 * @file
 * @brief Dynamic slab allocator
 *
 * @date 14.12.2010
 * @author Dmitry Zubarvich
 * @author Kirill Tyushev
 * @author Alexandr Kalmuk
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lib/list.h>
#include <util/binalign.h>
#include <kernel/printk.h>
#include <kernel/mm/slab.h>
#include <kernel/mm/kmalloc.h>
#include <kernel/mm/slab_statistic.h>
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

#if 0
# define SLAB_ALLOCATOR_DEBUG
#endif

extern char _heap_start;
extern char _heap_end;
# define HEAP_START_PTR 	(&_heap_start)
# define HEAP_END_PTR		(&_heap_end)

static page_info_t pages[CONFIG_HEAP_SIZE / CONFIG_PAGE_SIZE];

/* macros to finding the cache and slab which an obj belongs to */
#define SET_PAGE_CACHE(pg, x)  ((pg)->list.next = (struct list_head *)(x))
#define GET_PAGE_CACHE(pg)    ((cache_t *)(pg)->list.next)
#define SET_PAGE_SLAB(pg, x)   ((pg)->list.prev = (struct list_head *)(x))
#define GET_PAGE_SLAB(pg)     ((slab_t *)(pg)->list.prev)

/** max slab size in 2^n form */
#define MAX_SLAB_ORDER 3
/** max object size in 2^n form */
#define MAX_OBJ_ORDER 3
/** number for defining acceptable internal fragmentation */
#define MAX_INT_FRAGM_ORDER 8
/** size of kmalloc_cache in pages */
#define CACHE_CHAIN_SIZE 1
/** use to search a fit cache for object */
#define MAX_OBJECT_ALIGN 0

#ifdef SLAB_ALLOCATOR_DEBUG
void print_slab_info(cache_t *cachep, slab_t *slabp) {
	int free_elems_count = 0;
	struct list_head *elem;

	list_for_each(elem, &slabp->obj_ptr) {
		free_elems_count++;
	}
	printf("slabp->inuse: %d\n", slabp->inuse);
	printf("Number of objects allocated in the slab: %d\n\n",
			cachep->num - free_elems_count);
}
#endif

/* return information about page which an object belongs to */
static page_info_t* virt_to_page(void *objp) {
	unsigned int index = ((unsigned int) objp - (unsigned int) HEAP_START_PTR)
			/ CONFIG_PAGE_SIZE;
	return &(pages[index]);
}

/* main cache which will contain another descriptors of caches */
static cache_t cache_chain = {
	.name = "__cache_chain",
	.num = (CONFIG_PAGE_SIZE * CACHE_CHAIN_SIZE - binalign_bound(sizeof(slab_t), 4))
				/ binalign_bound(sizeof(cache_t), 4),
	.obj_size = binalign_bound(sizeof(cache_t), sizeof(struct list_head)),
	.slabs_full = {
		&cache_chain.slabs_full,
		&cache_chain.slabs_full
	},
        .slabs_free = {
    		&cache_chain.slabs_free,
    		&cache_chain.slabs_free
    	},
	.slabs_partial = {
		&cache_chain.slabs_partial,
		&cache_chain.slabs_partial
	},
	.next = {
		&cache_chain.next,
		&cache_chain.next
	},
	.growing = false,
	.slab_order = CACHE_CHAIN_SIZE
};

/**
 * Free memory which occupied by slab
 * @param slab_ptr the pointer to slab which must be deleted
 */
static void cache_slab_destroy(cache_t *cachep, slab_t *slabp) {
	mpfree(slabp);
}

/* init slab descriptor and slab objects */
static void cache_slab_init(cache_t *cachep, slab_t *slabp) {
	char* ptr_begin = (char*) slabp + binalign_bound(sizeof(slab_t), 4);
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
static int cache_grow(cache_t *cachep) {
	int pages_count;
	page_info_t *page;
	slab_t * slabp;
	size_t slab_size = 1 << cachep->slab_order;

	if (!(slabp = (slab_t*) mpalloc(slab_size)))
		return 0;

	page = virt_to_page(slabp);
	pages_count = slab_size;

	do {
		SET_PAGE_CACHE(page, cachep);
		SET_PAGE_SLAB(page, slabp);
		page++;
	} while (--pages_count);

	cache_slab_init(cachep, slabp);

	cachep->growing = true;
	list_add(&slabp->list, &cachep->slabs_free);

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
	size_t wastage = CONFIG_PAGE_SIZE << gfporder; /* total size being asked for */
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

cache_t *cache_create(char *name, size_t obj_size, size_t obj_num) {
	size_t left_over;
	cache_t *cachep;

	if (!name || strlen(name) >= __CACHE_NAMELEN - 1 || obj_size <= 0 || obj_size
			>= CONFIG_PAGE_SIZE << MAX_OBJ_ORDER)
		return NULL;

	cachep = (cache_t*) cache_alloc(&cache_chain);

	strcpy(cachep->name, name);
	cachep->obj_size = binalign_bound(obj_size, sizeof(struct list_head));
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

		if (left_over * MAX_INT_FRAGM_ORDER <= CONFIG_PAGE_SIZE
				<< cachep->slab_order)
			break; /* Acceptable internal fragmentation. */

		cachep->slab_order++;
	} while (1);

	if (!cachep->num)
		return NULL;

	cachep->growing = false;
	cachep->slabs_full.next = &(cachep->slabs_full);
	cachep->slabs_full.prev = &(cachep->slabs_full);
	cachep->slabs_partial.next = &(cachep->slabs_partial);
	cachep->slabs_partial.prev = &(cachep->slabs_partial);
	cachep->slabs_free.next = &(cachep->slabs_free);
	cachep->slabs_free.prev = &(cachep->slabs_free);
	list_add(&cachep->next, &(cache_chain.next));

#ifdef SLAB_ALLOCATOR_DEBUG
	printf("\n\nCreating cache with name \"%s\"\n", cachep->name);
	printf("Object size: %d\n", cachep->obj_size);
	printf("Slab order: %d\n", cachep->slab_order);
	printf("Number of objects in slab: %d\n", cachep->num);
	printf("Wastage: %d\n\n", left_over);
#endif

	return cachep;
}

void cache_destroy(cache_t *cachep) {
	struct list_head *ptr;
	slab_t * slabp;

	while (1) {
		ptr = cachep->slabs_free.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_free)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		cache_slab_destroy(cachep, slabp);
	}

	while (1) {
		ptr = cachep->slabs_full.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_full)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		cache_slab_destroy(cachep, slabp);
	}

	while (1) {
		ptr = cachep->slabs_partial.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_partial)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		cache_slab_destroy(cachep, slabp);
	}
	list_del(&cachep->next);
	cache_free(&cache_chain, cachep);

}

void *cache_alloc(cache_t *cachep) {
	slab_t * slabp;
	void *objp;

	/* getting slab */
	if (list_empty(&cachep->slabs_partial)) {
		if (list_empty(&cachep->slabs_free))
			if (!cache_grow(cachep))
				return NULL;
		slabp = list_entry(cachep->slabs_free.next, slab_t, list);
	} else {
		slabp = list_entry(cachep->slabs_partial.next, slab_t, list);
	}

	objp = slabp->obj_ptr.next;
	list_del(slabp->obj_ptr.next);

	slabp->inuse++;
	if (slabp->inuse == cachep->num) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_full);
	} else if (slabp->inuse == 1) {
		list_del(&slabp->list);
		list_add(&slabp->list, &cachep->slabs_partial);
	}

	cachep->growing = false;

#ifdef SLAB_ALLOCATOR_DEBUG
	printf("\n\nSlab info after allocating object:");
	print_slab_info(cachep, slabp);
#endif

	return objp;
}

void cache_free(cache_t *cachep, void* objp) {
	slab_t * slabp;
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

#ifdef SLAB_ALLOCATOR_DEBUG
	printf("\n\nSlab info after freeing object:");
	print_slab_info(cachep, slabp);
#endif
}

int cache_shrink(cache_t *cachep) {
	slab_t * slabp;
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
		cache_slab_destroy(cachep, slabp);
		ret++;
	}

	return ret;
}

/**
 * Search fit to this obj_size cache
 * @param obj_size is size for which is searching cache
 * @return pointer to this cache
 */
static cache_t *find_fit_cache(size_t obj_size) {
	struct list_head *tmp;
	cache_t *cachep;
	/* pointer to main cache */
	cache_t *cache_chainp;

	cache_chainp = &cache_chain;
	tmp = &cache_chainp->next;
	do {
		cachep = list_entry(tmp, cache_t, next);
		if (obj_size <= cachep->obj_size + MAX_OBJECT_ALIGN) {
			return cachep;
		}
		tmp = cachep->next.next;
	} while (tmp != &cache_chainp->next);

	return NULL;
}

void *kmalloc(size_t size) {
	cache_t *cachep;
	void *obj_ptr;
	char name[__CACHE_NAMELEN];

	/* different caches must be initialized with different names "__size" */
	sprintf(name, "%s\n", "__");
	sprintf(name + 2, "%d\n", size);
	cachep = find_fit_cache(size);
	if (cachep != NULL) {
		obj_ptr = cache_alloc(cachep);
		return obj_ptr;
	}
	/* if needed cache is not exist */
	cachep = cache_create(name, size, 0);
	obj_ptr = cache_alloc(cachep);

	return obj_ptr;
}

void kfree(void *obj) {
	page_info_t *page = virt_to_page(obj);
	cache_t *cachep = GET_PAGE_CACHE(page);
	cache_free(cachep, obj);
	cache_shrink(cachep);
}

void sget_blocks_info(struct list_head* list, struct list_head *slabp) {
	char *slab_begin;
	char *cur_objp;
	unsigned int num;
	struct list_head* cur_elem;
	slab_t * slab;
	block_info_t* tmp_info;
	page_info_t *page;
	cache_t *cachep;

	page = virt_to_page((void*) slabp);
	slab_begin = (char*) slabp + binalign_bound(sizeof(slab_t), 4);
	slab = list_entry(slabp, slab_t, list);
	cachep = GET_PAGE_CACHE(page);

	for (num = 0; num < cachep->num; num++) {
		cur_objp = slab_begin + num * cachep->obj_size;
		tmp_info = (block_info_t*) malloc(sizeof(block_info_t));
		tmp_info->size = cachep->obj_size;
		tmp_info->free = false;
		list_for_each(cur_elem, &slab->obj_ptr) {
			if ((char*) cur_elem == cur_objp) {
				tmp_info->free = true;
				break;
			}
		}
		list_add((struct list_head*) tmp_info, list);
	}
}

void make_caches_list(struct list_head* list) {
	list->next = &cache_chain.next;
	list->prev = cache_chain.next.prev;
}
