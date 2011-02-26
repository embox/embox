/**
 * @file
 * @brief Test slab allocator
 * @author Dmitry Zubarevich
 */

#include <stdio.h>

#include <embox/test.h>
#include <lib/list.h>
#include <kernel/mm/slab.h>

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */

struct list {
	struct list_head *next, *prev;
	int p;
};

static LIST_HEAD(int_list);

static int run(void) {
	int result = 0;
	struct list_head *tmp_h;
	int i;
	int *ptr_array;
	ptr_array = (int*) malloc(sizeof(int) * 10);
	ADD_CACHE(cache,int,0x100);

	TRACE("\t\t = Allocate list =\n");
	for (i = 0; i <= 10; i++) {
		ptr_array[i] = (int*) kmem_cache_alloc(&cache);
		if (ptr_array[i] == 0) {
			return -1;
		}
		TRACE("%d object is allocated\n",i);
	}
	i = 0;
	TRACE("\n\n\t\t = Test free =\n");
	for (i = 0; i <= 10; i++) {
		kmem_cache_free(&cache, (void*) ptr_array[i]);
		TRACE("%d object is freed\n",i);
	}
	i = 0;
	list_for_each(tmp_h, &((&cache)->obj_ptr)) {
		i++;
	}

	if (i != (&cache)->num) {
		TRACE("Error: no all are freed!");
		return -1;
	}

	free(ptr_array);
	TRACE("\ntest ");

	return result;
}

