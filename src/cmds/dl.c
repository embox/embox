/**
 * @file
 * @brief
 *
 * @date 26.10.2012
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

	elf_objlist_init(&list);

	for (int i = argc - 1; i >= 1; i--) {
		FILE *file = fopen(argv[i], "r");

		if (file == NULL) {
			printf("Cannot open file %s\n", argv[i]);
			return -EBADF;
		}

		if ((err = elf_object_init(&obj, file)) < 0 ) {
			return err;
		}

		elf_objlist_add(list, obj);
	}

	return elfloader_load(list);
}
