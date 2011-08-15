/**
 * @file
 *
 * @brief
 *
 * @date 13.08.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <stdio.h>
#include <framework/example/self.h>
#include <errno.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

struct example_array_registry_data {
	void (*run)(void);
	char name[0x10];
};

ARRAY_SPREAD_DEF(const struct example_array_registry_data,
		__example_array_registry);

#define array_registry_foreach(item_ptr) \
	array_foreach_ptr(item_ptr, __example_array_registry, \
		ARRAY_SPREAD_SIZE(__example_array_registry))

#define DEFINE_ITEM(item_name, func_run)                                      \
	/* we can declare item before it would be implement */                    \
	static void func_run(void);                                               \
	/* declare new item and  add new item to our array */                     \
	ARRAY_SPREAD_ADD(__example_array_registry, {                              \
		.run = func_run,                                                      \
		.name = "" item_name                                               \
	})


DEFINE_ITEM("first_item", first_run);
DEFINE_ITEM("second_item", second_run);
DEFINE_ITEM("thirst_item", third_run);

static void first_run(void) {
	printf("I'm first\n");
}

static void  second_run(void) {
	printf("I'm second\n");
}

static void  third_run(void) {
	printf("I'm third\n");
}

static int run(int argc, char **argv) {
	const struct example_array_registry_data *item_ptr;

	array_registry_foreach(item_ptr) {
		item_ptr->run();
	}

	return ENOERR;
}
