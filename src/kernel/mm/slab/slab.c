/**
 *@file
 *@brief slab allocator
 * some beginning in this way
 *
 *@date 27.11.2010
 *@author Alexandr Kalmuk
 *@author Kirill Tyushev
 *@author Dmitry Zubarevich
 */

#include <string.h>
#include <kernel/mm/slab.h>

/**
 * simple cache initialization
 * @param cache which will be initialize
 */
static void kmem_cache_init(kmem_cache_t* cache) {
	int count_elements = 0;
	struct list_head* elem;

	cache->obj_ptr.next = &(cache->obj_ptr);
	cache->obj_ptr.prev = &(cache->obj_ptr);
	elem = (struct list_head*)(cache->cache_begin);
	do {
		/*add this free block in slab_list*/
		list_add(elem, &(cache->obj_ptr));

		/*initialize (count_elements + 1)-<th> block*/
		count_elements++;
		elem = (struct list_head*) cache->cache_begin + cache->size * count_elements;

	} while (count_elements < cache->num);
}

void* kmem_cache_alloc(kmem_cache_t* cachep) {
	void* objp;

	if (!(cachep->hasinit)) {
		kmem_cache_init(cachep);
		cachep->hasinit = 1;
	}

	if (list_empty(&(cachep->obj_ptr)))
		return NULL;

	objp = cachep->obj_ptr.next;
	list_del(cachep->obj_ptr.next);

	return objp;
}

void  kmem_cache_free(kmem_cache_t* cachep, void* objp) {
	struct list_head* ptr_begin;

	if (objp == NULL)
		return;

	ptr_begin = (struct list_head*)(objp);
	list_add(ptr_begin, &(cachep->obj_ptr));
}


