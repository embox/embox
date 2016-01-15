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

static int ext2_getxattr(struct ext2_inode *node, const char *name,
		char *value, size_t size);

extern ext2_filsys fs; // from ext2fuse-src-0.8.1/src/fuse-ext2fs.c

void fuse_ext2_getxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
			size_t size) {
	int rc;
	int value_actual_size;
	struct ext2_inode inode;
	char *buf;

	buf = malloc(size);
	if (!buf) {
		return;
	}

	rc = read_inode(EXT2FS_INO(ino), &inode);
	if (rc) {
		fuse_reply_err(req, ENOENT);
		free(buf);
		return;
	}

	value_actual_size = ext2_getxattr(&inode, name, buf, size);
	fuse_reply_buf(req, buf, value_actual_size);
	free(buf);
}

static int ext2_getxattr(struct ext2_inode *node, const char *name,
		char *value, size_t size) {
	char *buf, *attr_start;
	struct ext2_ext_attr_entry *entry;
	char *entry_name;
	int res = -1;

	buf = malloc(fs->blocksize);
	if (!buf) {
		return res;
	}
	ext2fs_read_ext_attr(fs, node->i_file_acl, buf);

	attr_start = buf + sizeof(struct ext2_ext_attr_header);
	entry = (struct ext2_ext_attr_entry *) attr_start;

	while (!EXT2_EXT_IS_LAST_ENTRY(entry)) {
		entry_name = (char *)entry + sizeof(struct ext2_ext_attr_entry);

		if (entry->e_name_len == strlen(name)) {
			if (!strncmp(entry_name, name, entry->e_name_len)) {
				memcpy(value, buf + entry->e_value_offs, entry->e_value_size);
				res = entry->e_value_size;
				break;
			}
		}
		entry = EXT2_EXT_ATTR_NEXT(entry);
	}
	free(buf);
	return res;
}
