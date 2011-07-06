/**
 * @brief Implementation of cache_member_init function
 *
 * @date 6.07.2011
 * @author Alexandr Kalmuk
 */
#include <framework/mod/api.h>
#include <mem/misc/slab.h>

/** Inizialize cache according to storage data in info structure */
static int __cache_members_init(struct mod_members_info * info);

const struct mod_members_ops __cache_member_init = {
		.init = &__cache_members_init,
};

static int __cache_members_init(struct mod_members_info * info) {
	struct data *member_data = (struct data*)info->data;
	return cache_init(member_data->cache, member_data->obj_sz, member_data->obj_nr);
}
