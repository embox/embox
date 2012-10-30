/**
 * @file
 * @brief
 *
 * @date 30.10.2012
 * @author Anton Bulychev
 */

#include <types.h>
#include <stdlib.h>

#include <lib/libelf.h>

void elf_objlist_init(Elf32_Objlist *list) {
	list->first = NULL;
}

int elf_objlist_add(Elf32_Objlist *list, Elf32_Obj *obj) {
	Elf32_Objlist_item *item;

	if (!(item = malloc(sizeof(Elf32_Objlist_item)))) {
		return -ENOMEM;
	}

	*item = (Elf32_Objlist_item) {
		.obj  = obj,
		.next = list->first,
	};

	list->first = item;

	return ENOERR;
}

void elf_objlist_free_item(Elf32_Objlist_item *item) {
	free(item);
}

void elf_objlist_free(Elf32_Objlist *list) {
	Elf32_Objlist_item *next, *item = list->first;
	while (item) {
		next = item->next;
		elf_objlist_free_item(item);
		item = next;
	}
}


