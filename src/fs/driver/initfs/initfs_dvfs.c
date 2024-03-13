/**
 * @file
 * @details Read-only filesystem with direct address space mapping.
 *
 * @date 7 May 2015
 * @author Anton Bondarev
 *	        - initial implementation
 * @author Nikolay Korotky
 *	        - rework using vfs
 * @author Eldar Abusalimov
 *	        - rework mount to use cpio_parse_entry
 * @author Denis Deryugin
 *              - port from old VFS
 *
 * @note   Initfs is based on CPIO archive format. By design, this format
 *         has no directory abstraction, as all files are stored with full
 *         path names. Because of this it could be tricky to handle some
 *         VFS calls.
 */
#include <util/log.h>

#include <stdint.h>
#include <cpio.h>
#include <string.h>

#include <fs/inode.h>
#include <fs/super_block.h>

#include "initfs.h"

extern struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);

struct super_block_operations initfs_sbops = {
	.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = initfs_destroy_inode,
};
