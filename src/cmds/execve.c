/**
 * @file
 * @brief Execve execute program
 *
 * @date 27.09.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>

#include <getopt.h>

#include <lib/elfloader.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	int err;
	Elf32_Objlist *list;
	Elf32_Obj *obj;
	FILE *file = fopen(argv[argc - 1], "r");

	if (file == NULL) {
		printf("Cannot open file %s\n", argv[argc - 1]);
		return -EBADF;
	}

	if ((err = elf_object_init(&obj, file)) < 0 ) {
		return err;
	}

	elf_objlist_init(&list);

	elf_objlist_add(list, obj);

	return elfloader_load(list);
}
