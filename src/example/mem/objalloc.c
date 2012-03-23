/**
 * @file
 * @brief Example demonstrates using of API for allocation fixed size object
 *
 * @details To use pool write in conf/mods-kernel.conf
 *          mod(embox.mem.pool_adapter)
 *          To use slab write in conf/mods-kernel.conf
 *          mod(embox.mem.slab_adapter)
 *
 * @see include/mem/objalloc.h
 * @date 19.07.2011
 * @author Alexandr Kalmuk
 * @author Dmitry Zubarevich
 */

#include <stdio.h>
#include <framework/example/self.h>
#include <mem/objalloc.h>

/* Initial count of objects in the allocator */
#define TOTAL_OBJECTS 16

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

/** Struct of object you want to allocate in your allocator */
struct object_example {
	/* storage data */
	int data;
};

typedef struct object_example object_example_t;

/**
 *  Creates and initializes allocator.
 *  You can use your allocator immediately after it was created.
 *  @param allocator_name     - name of allocator
 *  @param struct object_example - type of objectects in allocator
 *  @param TOTAL_OBJECTS      - initial count of objects in allocator
 */
OBJALLOC_DEF(allocator_name, struct object_example, TOTAL_OBJECTS);

static int run(int argc, char **argv) {
	/* If you want to use function, but no macro */
	objalloc_t allocator;

	/* Allocate object in your allocator and use it after by object pointer */
	struct object_example *object = (struct object_example *) objalloc(
			&allocator_name);

	printf("Now you can use it!");
	if (!object) {
		printf("Object was not allocated!");
	}

	/* Use your object */
	object->data = 1;

	/* If your object is useless you can delete it.
	 * Memory for object will be free */
	objfree(&allocator_name, object);

	/* If your allocator is useless you can delete it */
	objalloc_destroy(&allocator_name);

	/* It is the other way to initialize allocator:
	 * Init your allocator and check if it was inited */
	if(!objalloc_init(&allocator, sizeof(struct object_example), TOTAL_OBJECTS)) {
		printf("Can not init allocator!");
	}

	/*
	 *  Now allocate and destroy your objects as above.
	 */

	/* If your allocator is useless you can delete it */
	objalloc_destroy(&allocator);

	return 0;
}
