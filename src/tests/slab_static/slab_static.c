/**
 * @file
 * @brief Test static slab allocator
 *
 * @date 29.11.10
 * @author Dmitry Zubarevich
 */

#include <stdio.h>
#include <embox/test.h>
#include <lib/list.h>
#include <kernel/mm/slab_static.h>

EMBOX_TEST(run);

STATIC_CACHE_CREATE(cache, int, 0x100);

static int run(void) {
	struct list_head *tmp_h;
	size_t i;
	int *ptr_array[11];

	TRACE("\t\t = Allocate list =\n");
	for (i = 0; i <= 10; i++) {
		ptr_array[i] = (int*) static_cache_alloc(&cache);
		if (ptr_array[i] == 0) {
			return -1;
		}
		TRACE("%d object is allocated\n",i);
	}
	TRACE("\n\n\t\t = Test free =\n");
	for (i = 0; i <= 10; i++) {
		static_cache_free(&cache, (void*) ptr_array[i]);
		TRACE("%d object is freed\n", i);
	}
	i = 0;
	list_for_each(tmp_h, &(cache.obj_ptr)) {
		i++;
	}

	if (i != (&cache)->num) {
		TRACE("Error: no all are freed!");
		return -1;
	}

	TRACE("\ntest ");
	return 0;
}

