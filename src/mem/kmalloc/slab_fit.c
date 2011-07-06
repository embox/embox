/**
 * @file
 * @brief Implemention of kmalloc API.
 *
 * @details kmalloc allocate object find most appropriate cache.
 * @date 26.03.11
 * @author Anton Bondarev
 * @author Alexandr Kalmuk
 * @author Kirill Tyushev
 * @author Dmitry Zubarevich
 */

#include <mem/misc/slab.h>
#include <mem/kmalloc.h>

extern char _heap_start;
extern char _heap_end;

typedef struct page_info {
	struct list_head list;
} page_info_t;

# define HEAP_START_PTR 	(&_heap_start)
# define HEAP_END_PTR		(&_heap_end)

/* how much difference may be between size of you object and
 * size of object in cache(cachep->obj_size) */
#define MAX_OBJECT_ALIGN 0

static page_info_t pages[CONFIG_HEAP_SIZE / CONFIG_PAGE_SIZE];

/* macros to finding the cache and slab which an obj belongs to */
#define SET_PAGE_CACHE(pg, x)  ((pg)->list.next = (struct list_head *)(x))
#define GET_PAGE_CACHE(pg)    ((cache_t *)(pg)->list.next)
#define SET_PAGE_SLAB(pg, x)   ((pg)->list.prev = (struct list_head *)(x))
#define GET_PAGE_SLAB(pg)     ((slab_t *)(pg)->list.prev)

/* return information about page which an object belongs to */
static page_info_t* virt_to_page(void *objp) {
	unsigned int index = ((unsigned int) objp - (unsigned int) HEAP_START_PTR)
			/ CONFIG_PAGE_SIZE;
	return &(pages[index]);
}

/* main cache which will contain another descriptors of caches */
extern cache_t cache_chain;

/**
 * Search fit to this obj_size cache.
 * your_object_size <= cachep->obj_size + MAX_OBJECT_ALIGN if and only if
 * cache is fit.
 *
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
	char name[__CACHE_NAMELEN ];

	/* different caches must be initialized with different names "__size" */
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
