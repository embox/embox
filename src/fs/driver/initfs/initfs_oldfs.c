/**
 * @file
 * @details Read-only filesystem with direct address space mapping.
 *
 * @date 29.06.09
 * @author Anton Bondarev
 *	        - initial implementation
 * @author Nikolay Korotky
 *	        - rework using vfs
 * @author Eldar Abusalimov
 *	        - rework mount to use cpio_parse_entry
 */

#include <stddef.h>

#include <fs/inode.h>
#include <fs/super_block.h>

#include "initfs.h"

struct super_block_operations initfs_sbops = {
	//.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = initfs_destroy_inode,
};
