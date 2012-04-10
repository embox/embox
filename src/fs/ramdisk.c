/**
 * @file ramdisk.c
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fs/ramfs.h>
#include <fs/fs.h>
#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/ramdisk.h>
#include <fs/fat.h>
#include <fs/driver_registry.h>
#include <util/array.h>
#include <embox/unit.h>
#include <stdlib.h>
#include <mem/page.h>
#include <cmd/mkfs.h>

static ramdisk_params_t ramd_params;

EMBOX_UNIT_INIT(unit_init);

int ramdisk_create(void *mkfs_params) {
	node_t *ramdisk_node;
	mkfs_params_t *p_mkfs_params;

	p_mkfs_params = (mkfs_params_t *)mkfs_params;

	if (NULL != (ramdisk_node = vfs_find_node(p_mkfs_params->name, NULL))) {
#ifdef _GAZ_DEBUG_
		printf("Ramdisk already exist 1\n");
#endif /*def _GAZ_DEBUG_ */
		return 0;/*file already exist*/
		}

	if(NULL == (ramd_params.start_addr = page_alloc(p_mkfs_params->blocks))) {
#ifdef _GAZ_DEBUG_
		printf("Out of memory\n");
#endif /*def _GAZ_DEBUG_ */
		return 0;
	}

	strcpy ((void *)&ramd_params.name, (const void *)p_mkfs_params->name);

	ramd_params.size = p_mkfs_params->blocks * CONFIG_PAGE_SIZE;
	ramd_params.blocks = p_mkfs_params->blocks;

	strcpy ((void *)&ramd_params.fs_name,
			(const void *)p_mkfs_params->fs_name);

	if (0 != fatfs_create((void *)&ramd_params)) {
#ifdef _GAZ_DEBUG_
		printf("Ramdisk already exist 2\n");
#endif /*def _GAZ_DEBUG_ */
		page_free(ramd_params.start_addr, p_mkfs_params->blocks);
		return 0;/*file already exist*/
	}

#ifdef _GAZ_DEBUG_
	printf("Create ramdisk %s, size %d, filesistem %s, addr %d,\n",	ramd_params.name,
			ramd_params.size, ramd_params.fs_name, (int)ramd_params.start_addr);
#endif /*def _GAZ_DEBUG_ */

#ifdef _GAZ_DEBUG_
	fat_main("/test1.txt");

	fat_main("/test2.txt");
#endif /*def _GAZ_DEBUG_ */

	return 0;

}

static int unit_init(void) {
	return 0;
}
