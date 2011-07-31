/**
 * @file
 * @brief Example demonstrates using of API for allocation fixed size object
 *
 * @details To use pool write in conf/mods-kernel.conf
 *          mod(embox.hal.mm.pool_adapter)
 *          To use slab write in conf/mods-kernel.conf
 *          mod(embox.hal.mm.slab_adapter)
 *
 * @see include/mem/objalloc.h
 * @date 19.07.2011
 * @author Alexandr Kalmuk
 * @author Dmitry Zubarevich
 */

#include <stdio.h>
#include <framework/example/self.h>
#include <mem/objalloc.h>

/* initial count of objectects in allocator */
#define TOTAL_OBJECTS 16

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

/* struct you want to allocate in your allocator */
struct object_example {
	/* storage data */
	int data;
};

typedef struct object_example object_example_t;

/**
 *  Create and initialize allocator.
 *  You can use your allocator immediately after it was created.
 *  @param allocator_name     - name of allocator
 *  @param struct object_example - type of objectects in allocator
 *  @param TOTAL_OBJECTS      - initial count of objectects in allocator
 */
OBJALLOC_DEF(allocator_name, struct object_example, TOTAL_OBJECTS);

static int run(int argc, char **argv) {
	/* allocate object in your allocator and use it after by pointer object */
	struct object_example *object = (struct object_example *) objalloc(
			&allocator_name);
	if (!object) {
		printf("Can not allocate!");
	}

	/* use your created object */
	object->data = 1;

	/* if your object is useless you can delete it.
	 * Memory for object will be free */
	objfree(&allocator_name, object);

	/* if your allocator is useless you can delete it */
	objalloc_destroy(&allocator_name);

	/* if you want to use function, but no macro */
	objalloc_t allocator;
	/* init your allocator and check if it was inited */
	if(!objalloc_init(&allocator, sizeof(struct object_example), TOTAL_OBJECTS)) {
		printf("Can not init allocator!");
	}

	/**
	 *  now allocate and destroy your objects as above.
	 */

	/* if your allocator is useless you can delete it */
	objalloc_destroy(&allocator);

	return 0;
}
