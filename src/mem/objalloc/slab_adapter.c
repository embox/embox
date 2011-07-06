/**
 * @brief Implementation of memory allocation API.
 *        It allocates object have some size in cache using slab allocator
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <mem/misc/slab.h>
#include <mem/objalloc.h>
#include <framework/mod/api.h>

void *objalloc(objalloc_t *allocator) {
	return cache_alloc(allocator);
}

void objfree(objalloc_t *allocator, void *object) {
	cache_free(allocator, object);
}

/** Inizialize cache according to storage data in info structure */
static int __cache_members_init(struct mod_members_info * info);

const struct mod_members_ops __cache_member_init = {
		.init = &__cache_members_init,
};

static int __cache_members_init(struct mod_members_info * info) {
	struct data *member_data = (struct data*)info->data;
	return cache_init(member_data->cache, member_data->obj_sz, member_data->obj_nr);
}
