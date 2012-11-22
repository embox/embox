/**
 * @file
 * @brief
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>

#include <getopt.h>

#include <stdlib.h>
#include <string.h>

#include <kernel/task.h>
#include <lib/elfloader.h>

EMBOX_CMD(exec);

static void *new_task_entry(void *file);

static int exec(int argc, char **argv) {
	// TODO: rewrite it

	char *filename = malloc(strlen(argv[argc - 1]) + 1);
	strcpy(filename, argv[argc - 1]);

	return new_task(new_task_entry, filename);
}

static void *new_task_entry(void *filename) {
	FILE *file = fopen(filename, "r");
	Elf32_Objlist *list;
	Elf32_Obj *obj;
	int err;

	free(filename);

	if (file == NULL) {
		printf("Cannot open file %s\n", (char *) filename);
		return NULL;
	}


	if ((err = elf_object_init(&obj, file)) < 0 ) {
		return NULL;
	}

	elf_objlist_init(&list);

	elf_objlist_add(list, obj);

	elfloader_load(list);

	fclose(file);

	return NULL;
}
