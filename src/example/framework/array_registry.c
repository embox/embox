/**
 * @file
 * @brief Demonstrates repository creation with ARRAY_SPREAD API
 * @details This example demonstrates work on repository which was create with
 *          ARRAY_SPREAD macro. You can use this technique for creation local
 *          repositories build during compilation and item for this repository
 *          can place in different files.
 *          This example use repository of item you can use repository of
 *          pointer with minimal changes if you want.
 *          First of all you must declare repository variable with macro
 *          ARRAY_SPREAD_DEF to create repository and declare type of item
 *          We use structure with two field (handler - for routing callback and
 *          name for search item in the repository)
 *          We also implemented several helpers (macro DEFINE_ITEM - for
 *          simplification declaration process and macro array_registry_foreach
 *          for iterates over the repository.
 *          We added utility function lookup_in_repo as helper for search item
 *          in the repository. We use item's name as a identifier.
 *
 * @date 13.08.2011
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <framework/example/self.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

/** Declare data type for repository */
struct example_array_registry_data {
	void (*run)(void); /**< handle function */
	char name[0x10];   /**< name of item, you can find needed element in repo */
};

/**
 * Macro declares a repository's variable.
 * You can add an item to the array with ARRAY_SPREAD_ADD or
 * ARRAY_SPREAD_ADD_NAMED macros
 */
ARRAY_SPREAD_DEF(const struct example_array_registry_data,
		__example_array_registry);

/** Utility macro. It makes easier iterates over the repository */
#define array_registry_foreach(item) \
	array_spread_foreach_ptr(item, __example_array_registry)
/**
 * Macro for item declaration.
 * It allocates structure of item and fills its field. Then it put item to the
 * repository with macro ARRAY_SPREAD_ADD.
 * You can also split adding and allocation if you use ARRAY_SPREAD_ADD_NAMED
 * macro.
 * You also can put not structure to repository array but pointer to it if you
 * declare array with pointer type.
 */
#define DEFINE_ITEM(item_name, func_run) \
	/* we can declare item before it would be implement */  \
	static void func_run(void);                             \
	/* declare new item and  add new item to our array */   \
	ARRAY_SPREAD_ADD(__example_array_registry, {            \
		.run = func_run,                                \
		.name = "" item_name                            \
	})

static struct example_array_registry_data *lookup_in_repo(char *name,
		struct example_array_registry_data *buff) {
	struct example_array_registry_data item;

	array_registry_foreach(item) {
		if (0 == strncmp(item.name, name, sizeof(item.name))) {
			memcpy(buff, &item, sizeof(item));
			return buff;
		}
	}
	return NULL;
}

/** declare first item and add it to the repo */
DEFINE_ITEM("first_item", first_run);
/** declare second item and add it to the repo */
DEFINE_ITEM("second_item", second_run);
/** declare third item and add it to the repo */
DEFINE_ITEM("thirst_item", third_run);

/** handle for first item */
static void first_run(void) {
	printf("I'm first\n");
}

/** handle for second item */
static void  second_run(void) {
	printf("I'm second\n");
}

/** handle for third item */
static void  third_run(void) {
	printf("I'm third\n");
}

/**
 * Example's execution routing
 * It has been declare with macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	/* temporary variable for iterates over the repository */
	struct example_array_registry_data item;

	/* iterate over repository and call handler for each element */
	array_registry_foreach(item) {
		item.run();
	}
	lookup_in_repo("second_item", &item);
	item.run();

	return ENOERR;
}
