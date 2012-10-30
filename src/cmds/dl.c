/**
 * @file
 * @brief Loadreloc loads relocatable ELF binaries.
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdio.h>

#include <lib/elfloader.h>
#include <lib/libelf.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	Elf32_Objlist *list;
	FILE *file1 = fopen("reloc1.o", "r");
	FILE *file2 = fopen("reloc2.o", "r");
	FILE *file3 = fopen("libtest.so", "r");
	Elf32_Obj *obj1, *obj2, *obj3;

	elf_objlist_init(&list);

	elf_object_init(&obj1, file1);
	elf_object_init(&obj2, file2);
	elf_object_init(&obj3, file3);

	elf_objlist_add(list, obj1);
	elf_objlist_add(list, obj2);
	elf_objlist_add(list, obj3);

	return elfloader_load(list);

	//elf_objlist_free(&list);
}
