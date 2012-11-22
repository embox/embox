/**
 * @file
 * @brief
 *
 * @date 30.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <assert.h>

#include <lib/libelf.h>
#include <lib/libdl.h>
#include <lib/elfloader.h>

#include <kernel/task.h>
#include <kernel/usermode.h>
#include <mem/mmap.h>

static int elf_load_in_mem(Elf32_Obj *obj) {
	switch(obj->header->e_type) {
	case (ET_REL):
		return elfloader_place_relocatable(obj);
	case (ET_DYN):
		return elfloader_place_shared(obj);
	case (ET_EXEC):
		return elfloader_place_executable(obj);
	default:
		return -ENOSYS;
	}
}

uint32_t elfloader_find_entrypoint(Elf32_Objlist *list) {
	Elf32_Objlist_item *item = list->first;
	Elf32_Obj *obj;

	while (item) {
		obj = item->obj;

		if (obj->header->e_type == ET_EXEC) {
			return obj->header->e_entry;
		}

		item = item->next;
	}

	return 0;
}

int elfloader_load(Elf32_Objlist *list) {
	Elf32_Objlist_item *item = list->first;
	dl_data *data;
	struct ue_data ue_data;

	int err;

	while (item) {
		if ((err = elf_load_in_mem(item->obj)) < 0) {
			return err;
		}
		item = item->next;
	}

	if ((err = dl_proceed(list, &data)) < 0) {
		return err;
	}

	ue_data.ip = (void *) elfloader_find_entrypoint(list);
	ue_data.sp = (void *) mmap_create_stack(task_self()->mmap);

	dl_free_data(data);

	usermode_entry(&ue_data);

	return ENOERR;
}
