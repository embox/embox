/**
 * @file
 * @brief Test slab allocator
 *
 * @date 28.11.10
 * @author Dmitry Zubarevich
 */

#include <stdio.h>
#include <embox/test.h>
#include <lib/list.h>
#include <kernel/mm/slab.h>

EMBOX_TEST(run);

static int run(void) {
	int result = 0;

	int *obj_1_1;
	int *obj_1_2;
	int *obj_1_3;
	int *obj_1_4;

	double *obj_2_1;
	double *obj_2_2;
	double *obj_2_3;
	double *obj_2_4;

	char *obj_3_1;
	char *obj_3_2;
	char *obj_3_3;
	char *obj_3_4;

	kmem_cache_t* cache1 = get_cache("cache1");
	kmem_cache_t* cache2 = get_cache("cache2");
	kmem_cache_t* cache3 = get_cache("cache3");

	obj_1_1 = (int*) kmem_cache_alloc(cache1);
	obj_1_2 = (int*) kmem_cache_alloc(cache1);
	obj_1_3 = (int*) kmem_cache_alloc(cache1);
	obj_1_4 = (int*) kmem_cache_alloc(cache1);

	/* WARNING: be carefull, double type maybe not supported */
	obj_2_1 = (double*) kmem_cache_alloc(cache2);
	obj_2_2 = (double*) kmem_cache_alloc(cache2);
	obj_2_3 = (double*) kmem_cache_alloc(cache2);
	obj_2_4 = (double*) kmem_cache_alloc(cache2);

	obj_3_1 = (char*) kmem_cache_alloc(cache3);
	obj_3_2 = (char*) kmem_cache_alloc(cache3);
	obj_3_3 = (char*) kmem_cache_alloc(cache3);
	obj_3_4 = (char*) kmem_cache_alloc(cache3);

	kmem_cache_free(cache1, obj_1_1);
	kmem_cache_free(cache1, obj_1_2);
	kmem_cache_free(cache1, obj_1_3);
	kmem_cache_free(cache1, obj_1_4);

	kmem_cache_free(cache2, obj_2_1);
	kmem_cache_free(cache2, obj_2_2);
	kmem_cache_free(cache2, obj_2_3);
	kmem_cache_free(cache2, obj_2_4);

	kmem_cache_free(cache3, obj_3_1);
	kmem_cache_free(cache3, obj_3_2);
	kmem_cache_free(cache3, obj_3_3);
	kmem_cache_free(cache3, obj_3_4);

	return result;
}
