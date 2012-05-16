/**
 * @file ramdisk.c
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <types.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fs/fs_drv.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/ramdisk.h>
#include <fs/fat.h>
#include <util/array.h>
#include <embox/unit.h>
#include <stdlib.h>
#include <mem/page.h>
#include <cmd/mkfs.h>

typedef struct ramdisk_params_head {
	struct list_head *next;
	struct list_head *prev;
	ramdisk_params_t param;
} ramdisk_params_head_t;

static ramdisk_params_head_t ramdisk_pool[QUANTITY_RAMDISK];
static LIST_HEAD(ramdisk_free);

#define param_to_head(fparam) \
	(uint32_t)(fparam - offsetof(ramdisk_params_head_t, param))

static void init_ramdisk_info_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(ramdisk_pool); i++) {
		list_add((struct list_head *) &ramdisk_pool[i], &ramdisk_free);
	}
}

static ramdisk_params_t *ramdisk_info_alloc(void) {
	ramdisk_params_head_t *head;
	ramdisk_params_t *param;

	if (list_empty(&ramdisk_free)) {
		return NULL;
	}
	head = (ramdisk_params_head_t *) (&ramdisk_free)->next;
	list_del((&ramdisk_free)->next);
	param = &(head->param);
	return param;
}

static void ramdisk_info_free(ramdisk_params_t *param) {
	if (NULL == param) {
		return;
	}
	list_add((struct list_head*) param_to_head(param), &ramdisk_free);
}

EMBOX_UNIT_INIT(unit_init);

int ramdisk_create(void *mkfs_params) {
	node_t *ramdisk_node;
	mkfs_params_t *p_mkfs_params;
	ramdisk_params_t *ramd_params;

	p_mkfs_params = (mkfs_params_t *)mkfs_params;

	if (NULL == (ramdisk_node = vfs_add_path(p_mkfs_params->path, NULL))) {
		return -EBUSY;/*file already exist*/
	}

	ramd_params = ramdisk_info_alloc();
	ramdisk_node->attr = (void *) ramd_params;

	if(NULL == (ramd_params->p_start_addr =
			page_alloc(p_mkfs_params->blocks))) {
		return -ENOMEM;
	}

	strcpy ((void *)&ramd_params->path, (const void *)p_mkfs_params->path);
	ramd_params->size = p_mkfs_params->blocks * CONFIG_PAGE_SIZE;
	ramd_params->blocks = p_mkfs_params->blocks;

	strcpy ((void *)&ramd_params->fs_name,
			(const void *)p_mkfs_params->fs_name);

	return 0;
}

ramdisk_params_t *ramdisk_get_param(char *name) {
	node_t *ramdisk_node;

	if (NULL == (ramdisk_node = vfs_find_node(name, NULL))) {
		return NULL;
	}
	return (ramdisk_params_t *) ramdisk_node->attr;
}

int ramdisk_delete(const char *name) {
	node_t *ramdisk_node;
	ramdisk_params_t *ramd_params;

	if (NULL == (ramdisk_node = vfs_find_node(name, NULL))) {
		return -1;
	}
	ramd_params = ramdisk_node->attr;

	ramdisk_info_free(ramd_params);
	vfs_del_leaf(ramdisk_node);
	return 0;
}

static int unit_init(void) {
	init_ramdisk_info_pool();
	return 0;
}
