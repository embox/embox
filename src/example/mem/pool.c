/**
 * @file
 * @brief	Example working with fixed-size pool with fixed size objects.
 *
 * @date	14.11.11
 * @author	Gleb Efimov
 *
 */

#include <framework/example/self.h>
#include <mem/misc/pool.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);


struct example_type {
	int data;
};

#define MY_POOL_SZ 100

POOL_DEF(my_pool, struct example_type, MY_POOL_SZ);

static int run(int argc, char **argv) {
	struct example_type* temp[MY_POOL_SZ];

	for (int i = 0; i < MY_POOL_SZ; i++) {
		temp[i] = pool_alloc(&my_pool);
		temp[i]->data = i;
	}

	for (int i = 0; i < MY_POOL_SZ; i++) {
		pool_free(&my_pool, temp[i]);
	}

	return 0;
}
