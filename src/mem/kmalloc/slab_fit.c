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
#include <mem/page.h>
#include <mem/heap.h>
#include <embox/unit.h>
#include <util/dlist.h>

EMBOX_UNIT_INIT(heap_init);

#define HEAP_SIZE OPTION_MODULE_GET(embox__mem__kmalloc,NUMBER,heap_size)

/* some information about page  */
typedef struct page_info {
	cache_t *cache;
} page_info_t;

static char *HEAP_START_PTR;
static char *HEAP_END_PTR;

/* how much difference may be between size of you object and
 * size of object in cache(cachep->obj_size) */
#define MAX_OBJECT_ALIGN 0

static page_info_t pages[HEAP_SIZE / PAGE_SIZE()];

/* macros to finding the cache and slab which an obj belongs to */
#define SET_PAGE_CACHE(pg, x)  ((pg)->cache = (x))
#define GET_PAGE_CACHE(pg)    ((pg)->cache)

/* return information about page which an object belongs to */
static page_info_t* ptr_to_page(void *objp) {
	unsigned int index = ((unsigned int) objp - (unsigned int) HEAP_START_PTR)
			/ PAGE_SIZE();
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
	struct dlist_head *tmp;
	cache_t *cachep;
	/* pointer to main cache */
	cache_t *cache_chainp;

	cache_chainp = &cache_chain;
	tmp = &cache_chainp->next;
	do {
		cachep = dlist_entry(tmp, cache_t, next);
		if (obj_size <= cachep->obj_size + MAX_OBJECT_ALIGN) {
			return cachep;
		}
		tmp = cachep->next.next;
	} while (tmp != &cache_chainp->next);

	return NULL;
}

void *kmalloc(size_t size, int priority) {
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
	page_info_t *page = ptr_to_page(obj);
	cache_t *cachep = GET_PAGE_CACHE(page);
	cache_free(cachep, obj);
	cache_shrink(cachep);
}

static int heap_init(void) {
	extern struct page_allocator *__heap_pgallocator;
	int page_cnt = (HEAP_SIZE / PAGE_SIZE() - 2);

	HEAP_START_PTR = page_alloc(__heap_pgallocator, page_cnt);
	if(NULL == HEAP_START_PTR) {
		return -1;
	}
	HEAP_END_PTR = HEAP_START_PTR + page_cnt * PAGE_SIZE();

	return 0;
}
