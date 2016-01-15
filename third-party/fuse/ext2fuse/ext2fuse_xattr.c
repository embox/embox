/**
 * @file
 * @brief ext xattr support
 *
 * @date 14.01.2015
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>

#include <fs/dvfs.h>

// Needed by fuse_common.h
#define _FILE_OFFSET_BITS 64

// it is from fuse-ext2fs.c
#define FUSE_USE_VERSION 25
#include <fuse_lowlevel.h> // fuse
#include <fuse_opt.h> // fuse
#include <fuse_kernel.h> // fuse

#define EXT2_FLAT_INCLUDES 0

#include <ext2fs/ext2fs.h> // e2fsprogs
#include <ext2fs/ext2_fs.h> // e2fsprogs
#include <ext2fs.h> // ext2fuse

#define EXT2_MAX_NAMELEN 255

static void ext2_getxattr(struct ext2_inode *node);

extern ext2_filsys fs; // from ext2fuse-src-0.8.1/src/fuse-ext2fs.c

void ext2fuse_getxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
			size_t size) {
	int rc;
	struct ext2_inode inode;

	rc = read_inode(EXT2FS_INO(ino), &inode);
	if (rc) {
		fuse_reply_err(req, ENOENT);
		return;
	}
	
	ext2_getxattr(&inode);
}

static void ext2_getxattr(struct ext2_inode *node) {
	struct ext2_inode_large *inode;
	struct ext2_ext_attr_entry *entry;
	char *start;
	unsigned int storage_size, remain;

	inode = (struct ext2_inode_large *) node;
	storage_size = EXT2_INODE_SIZE(fs->super) - EXT2_GOOD_OLD_INODE_SIZE -
		inode->i_extra_isize;
	start = ((char *) inode) + EXT2_GOOD_OLD_INODE_SIZE +
		inode->i_extra_isize + sizeof(__u32);
	entry = (struct ext2_ext_attr_entry *) start;

	remain = storage_size - sizeof(__u32);

	(void) inode;
	(void) start;
	(void) entry;
	(void) storage_size;
	(void) remain;
}
