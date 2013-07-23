/**
 * @file
 *
 * @brief File System Driver wrapper for NTFS
 *
 * @date Jul 23, 2013
 * @author: inconnu
 */

#include <fs/fs_driver.h>


#include <time.h>
#define __timespec_defined

static inline void __x86_verificator__(void) {
	// This is to make sure this file only compiles on x86
	asm ("mov %%cr2, %%eax");
}
#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321
#define	__PDP_ENDIAN	3412
// x86 architecture only
#define __BYTE_ORDER __LITTLE_ENDIAN

#include <ntfs-3g/volume.h>



static int embox_ntfs_node_create(struct node *parent_node, struct node *new_node) {
	return 0;
}

static int embox_ntfs_node_delete(struct node *nod) {
	return 0;
}

static int embox_ntfs_mount(void *dev_node, void *dir_node) {
	ntfs_volume *vol;

	vol = ntfs_mount("/dev/null",0);
	(void)vol;

	return 0;
}

static const struct fsop_desc ntfs_fsop = {
	.create_node = embox_ntfs_node_create,
	.delete_node = embox_ntfs_node_delete,
	.mount = embox_ntfs_mount,
};

static const struct fs_driver ntfs_driver = {
	.name = "ntfs",
	.file_op = NULL,
	.fsop = &ntfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ntfs_driver);
