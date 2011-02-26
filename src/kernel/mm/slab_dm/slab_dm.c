/**
 * @file
 * @brief Dynamic SLAB allocator
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
#define GET_PAGE_CACHE(pg)    ((kmem_cache_t *)(pg)->list.next)
#define SET_PAGE_SLAB(pg, x)   ((pg)->list.prev = (struct list_head *)(x))
#define GET_PAGE_SLAB(pg)     ((slab_t *)(pg)->list.prev)

/*use to search a fit cache for object*/
#define MAX_OBJECT_ALIGN 0

#ifdef SLAB_ALLOCATOR_DEBUG
void print_slab_info(kmem_cache_t *cachep, slab_t *slabp) {
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

/**
 * Main cache which will contain another descriptors of caches
 */
static kmem_cache_t kmalloc_cache = {
	.name = "__kmalloc_cache",
	.num = (CONFIG_PAGE_SIZE * KMALLOC_CACHE_SIZE - CACHE_ALIGN(sizeof(slab_t)))
				/ CACHE_ALIGN(sizeof(kmem_cache_t)),
	.obj_size = ALIGN_UP(sizeof(kmem_cache_t), sizeof(struct list_head)),
	.slabs_full = {
		&kmalloc_cache.slabs_full,
		&kmalloc_cache.slabs_full
	},
        .slabs_free = {
    		&kmalloc_cache.slabs_free,
    		&kmalloc_cache.slabs_free
    	},
	.slabs_partial = {
		&kmalloc_cache.slabs_partial,
		&kmalloc_cache.slabs_partial
	},
	.next = {
		&kmalloc_cache.next,
		&kmalloc_cache.next
	},
	.growing = false,
	.gfporder = KMALLOC_CACHE_SIZE
};

/**
 * Free memory which occupied by slab
 * @param slab_ptr the pointer to slab which must be deleted
 */
static void kmem_dmcache_slab_destroy(kmem_cache_t *cachep, slab_t *slabp) {
	mpfree(slabp);
}

/* init slab descriptor and slab objects */
static void kmem_dmcache_slab_init(kmem_cache_t *cachep, slab_t *slabp) {
	char* ptr_begin = (char*) slabp + CACHE_ALIGN(sizeof(slab_t));
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
	slab_t * slabp;
	size_t slab_size = 1 << cachep->gfporder;

	if (!(slabp = (slab_t*) mpalloc(slab_size)))
		return 0;

	page = virt_to_page(slabp);
	pages_count = slab_size;

	do {
		SET_PAGE_CACHE(page, cachep);
		SET_PAGE_SLAB(page, slabp);
		page++;
	} while (--pages_count);

	kmem_dmcache_slab_init(cachep, slabp);

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
static void kmem_dmcache_estimate(unsigned int gfporder, size_t size,
		size_t *left_over, unsigned int *num) {
	int count;
	size_t wastage = CONFIG_PAGE_SIZE << gfporder; /* total size being asked for */
	size_t base = 0;

	base = sizeof(slab_t);

	/* calculate the number of objects that will fit inside the slab, including the
	 * base slab_t */
	count = 0;
	while (count * size + CACHE_ALIGN(base) <= wastage)
		count++;
	if (count > 0)
		count--;

	/* return number objects that fit, and total space wasted */
	*num = count;
	wastage -= count * size;
	wastage -= CACHE_ALIGN(base);
	*left_over = wastage;
}

kmem_cache_t *kmem_dmcache_create(char *name, size_t obj_size) {
	size_t left_over;
	kmem_cache_t *cachep;

	if (!name || strlen(name) >= CACHE_NAMELEN - 1 || obj_size <= 0 || obj_size
			>= CONFIG_PAGE_SIZE << MAX_OBJ_ORDER)
		return NULL;

	cachep = (kmem_cache_t*) kmem_dmcache_alloc(&kmalloc_cache);

	strcpy(cachep->name, name);
	cachep->obj_size = ALIGN_UP(obj_size, sizeof(struct list_head));
	cachep->gfporder = 0;

	do {
		kmem_dmcache_estimate(cachep->gfporder, cachep->obj_size, &left_over,
				&cachep->num);

		if (cachep->gfporder >= MAX_GFP_ORDER) /* order == 3, 8 pages */
			break;

		if (!cachep->num) { /* we could not fit any objects on slab */
			cachep->gfporder++;
			continue;
		}

		if (left_over * MAX_INTFRAGM_ORDER <= CONFIG_PAGE_SIZE
				<< cachep->gfporder)
			break; /* Acceptable internal fragmentation. */

		cachep->gfporder++;
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
	list_add(&cachep->next, &(kmalloc_cache.next)); //////!!!!!!!!!!!!!

#ifdef SLAB_ALLOCATOR_DEBUG
	printf("\n\nCreating cache with name \"%s\"\n", cachep->name);
	printf("Object size: %d\n", cachep->obj_size);
	printf("Gfporder: %d\n", cachep->gfporder);
	printf("Number of objects in slab: %d\n", cachep->num);
	printf("Wastage: %d\n\n", left_over);
#endif

	return cachep;
}

void kmem_dmcache_destroy(kmem_cache_t *cachep) {
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
		kmem_dmcache_slab_destroy(cachep, slabp);
	}

	while (1) {
		ptr = cachep->slabs_full.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_full)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		kmem_dmcache_slab_destroy(cachep, slabp);
	}

	while (1) {
		ptr = cachep->slabs_partial.prev;
		/*if list is empty*/
		if (ptr == &cachep->slabs_partial)
			break;
		/* remove this slab from the list */
		slabp = list_entry(ptr, slab_t, list);
		list_del(&slabp->list);
		kmem_dmcache_slab_destroy(cachep, slabp);
	}
	list_del(&cachep->next);
	kmem_dmcache_free(&kmalloc_cache, cachep);

}

void *kmem_dmcache_alloc(kmem_cache_t *cachep) {
	slab_t * slabp;
	void *objp;

	/* getting slab */
	if (list_empty(&cachep->slabs_partial)) {
		if (list_empty(&cachep->slabs_free))
			if (!kmem_dmcache_grow(cachep))
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

void kmem_dmcache_free(kmem_cache_t *cachep, void* objp) {
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

int kmem_dmcache_shrink(kmem_cache_t *cachep) {
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
		kmem_dmcache_slab_destroy(cachep, slabp);
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
		cachep = list_entry(tmp, kmem_cache_t, next);
		if (obj_size <= cachep->obj_size + MAX_OBJECT_ALIGN && cachep->obj_size
				<= obj_size) {
			return cachep;
		}
		tmp = cachep->next.next;
	} while (tmp != &kmalloc_cachep->next);

	return NULL;
}

void *smalloc(size_t size) {
	kmem_cache_t *cachep;
	void *obj_ptr;
	char name[CACHE_NAMELEN];

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

void sfree(void *obj) {
	page_info_t *page = virt_to_page(obj);
	kmem_cache_t *cachep = GET_PAGE_CACHE(page);
	kmem_dmcache_free(cachep, obj);
}

void sget_blocks_info(struct list_head* list, struct list_head *slabp) {
	char *slab_begin;
	char *cur_objp;
	unsigned int num;
	struct list_head* cur_elem;
	slab_t * slab;
	block_info_t* tmp_info;
	page_info_t *page;
	kmem_cache_t *cachep;

	page = virt_to_page((void*) slabp);
	slab_begin = (char*) slabp + CACHE_ALIGN(sizeof(slab_t));
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
	list->next = &(kmalloc_cache.next);
	list->prev = (&(kmalloc_cache.next))->prev;
}
