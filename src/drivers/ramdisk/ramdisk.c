/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <string.h>
#include <cmd/mkfs.h>
#include <embox/unit.h>
#include <fs/vfs.h>
#include <drivers/ramdisk.h>
#include <mem/page.h>
#include <mem/misc/pool.h>

typedef struct ramdisk_params_head {
	struct list_head *next;
	struct list_head *prev;
	ramdisk_params_t param;
} ramdisk_params_head_t;

POOL_DEF ( ramdisk_pool, struct ramdisk_params_head, OPTION_GET(NUMBER,ramdisk_quantity ));

EMBOX_UNIT_INIT(unit_init);

int ramdisk_create(void *mkfs_params) {
	node_t *ramdisk_node;
	mkfs_params_t *p_mkfs_params;
	ramdisk_params_t *ramd_params;

	p_mkfs_params = (mkfs_params_t *)mkfs_params;

	if (NULL == (ramdisk_node = vfs_add_path(p_mkfs_params->path, NULL))) {
		return -EBUSY;/*file already exist*/
	}

	ramd_params = pool_alloc(&ramdisk_pool);
	ramdisk_node->attr = (void *) ramd_params;

	if(NULL == (ramd_params->p_start_addr =
			page_alloc(p_mkfs_params->blocks))) {
		return -ENOMEM;
	}

	strcpy ((void *)&ramd_params->path, (const void *)p_mkfs_params->path);
	ramd_params->size = p_mkfs_params->blocks * PAGE_SIZE();
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

	pool_free(&ramdisk_pool, ramd_params);
	vfs_del_leaf(ramdisk_node);
	return 0;
}

static int unit_init(void) {
	//init_ramdisk_info_pool();
	return 0;
}
