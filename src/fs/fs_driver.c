/**
 * @file
 *
 * @date 06.10.2010
 * @author Anton Bondarev
 */

#include <fs/fs.h>
#include <embox/kernel.h>
#include <embox/unit.h>
#include <lib/list.h>


static file_system_driver_t pool[4];
static LIST_HEAD(free_list);

EMBOX_UNIT_INIT(unit_init);

static int unit_init(void) {
	int i;
	for(i = 0; i < array_len(pool); i ++) {
		list_add((struct list_head *)&pool[i], &free_list);
	}
	return 0;
}

file_system_driver_t *alloc_fs_drivers(void) {
	if(list_empty(&free_list))
		return NULL;
	list_del_init(&free_list);

	return NULL;
}

void free_fs_drivers(file_system_driver_t *fs_drv) {
	if(NULL == fs_drv) {
		return;
	}
	list_add((struct list_head *)fs_drv, &free_list);

}

