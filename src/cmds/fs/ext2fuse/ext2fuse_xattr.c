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
static int ext2_setxattr(struct ext2_inode *node, const char *name,
		const char *value, size_t size, int flags);

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

void fuse_ext2_setxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
              const char *value, size_t size, int flags) {
	int rc;
	int written;
	struct ext2_inode inode;

	rc = read_inode(EXT2FS_INO(ino), &inode);
	if (rc) {
		fuse_reply_err(req, ENOENT);
		return;
	}

	written = ext2_setxattr(&inode, name, value, size, flags);
	write_inode(EXT2FS_INO(ino), &inode);
	fuse_reply_write(req, written);
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

static blk_t ext2_alloc_xattr_block(struct ext2_inode *node) {
	blk_t blknr;

	ext2fs_new_block(fs, 0, 0, &blknr);
	if (blknr > 0) {
		node->i_file_acl = blknr;
	}

	return blknr;
}

static int ext2_setxattr(struct ext2_inode *node, const char *name,
		const char *value, size_t size, int flags) {
	char *buf, *attr_start;
	struct ext2_ext_attr_entry *entry;
	struct ext2_ext_attr_header *header;
	blk_t blknr;
	// The place within a block from where values start
	uint32_t e_value_offs = fs->blocksize;

	buf = malloc(fs->blocksize);
	if (!buf) {
		return -1;
	}

	if (!node->i_file_acl) {
		blknr = ext2_alloc_xattr_block(node);
		if (blknr < 0) {
			free(buf);
			return -1;
		}
	}

	ext2fs_read_ext_attr(fs, node->i_file_acl, buf);

	header = (struct ext2_ext_attr_header*)buf;
	attr_start = buf + sizeof(struct ext2_ext_attr_header);
	entry = (struct ext2_ext_attr_entry *) attr_start;

	while (!EXT2_EXT_IS_LAST_ENTRY(entry)) {
		e_value_offs = min(e_value_offs, entry->e_value_offs);

		entry = EXT2_EXT_ATTR_NEXT(entry);
	}
	entry->e_name_len = strlen(name);
	entry->e_name_index = 1;
	entry->e_value_size = size;
	entry->e_value_offs = e_value_offs - entry->e_value_size;
	strcpy(EXT2_EXT_ATTR_NAME(entry), name);
	*(uint32_t*)EXT2_EXT_ATTR_NEXT(entry) = 0;
	//entry->e_hash = ext2fs_ext_attr_hash_entry(entry, (char *)value);

	/* FIXME: Use ext2fs_adjust_ea_refcount */
	header->h_magic = EXT2_EXT_ATTR_MAGIC;
	header->h_refcount = 1;
	header->h_blocks = 1;

	// TODO: Check that entries do not intersect with values

	memcpy(buf + entry->e_value_offs, value, entry->e_value_size);
	ext2fs_write_ext_attr2(fs, node->i_file_acl, buf);

	free(buf);
	return size;
}
