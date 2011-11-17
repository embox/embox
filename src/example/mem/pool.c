/**
 * @file
 * @brief	test file for object pool allocator
 *
 * @date	14.11.11
 * @author	Gleb Efimov
 *
 */

#include <framework/example/self.h>
#include <mem/misc/pool2.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);


struct example_type {
	int temp;
};

#define MY_POOL_SZ 100

//POOL_DEF(my_pool,struct example_type et, MY_POOL_SZ);

struct example_type my_storage[MY_POOL_SZ];

struct pool my_pool = {
	.memory = (void*)my_storage,
	.obj_size = sizeof(struct example_type),
	.pool_size = sizeof(struct example_type) * MY_POOL_SZ,
	.bound_free = my_storage
};

static int run(int argc, char **argv) {
	return 0;
}
