/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <stddef.h>

#include <fs/inode.h>

#include <fs/ext2.h>

#include <fs/file_desc.h>

#include <util/err.h>

extern int ext2_buf_read_file(struct inode *inode, char **, size_t *);
extern int ext2_read_inode(struct inode *node, uint32_t);
extern size_t ext2_write_file(struct inode *inode, char *buf_p, size_t size);
extern int ext2_close(struct inode *node);

/*
 * file_operation
 */
static struct idesc *ext2fs_open(struct inode *node, struct idesc *idesc, int __oflag) {
	int rc;
	struct ext2_file_info *fi;
	//struct ext2_fs_info *fsi;

	fi = inode_priv(node);
	//fsi = node->i_sb->sb_data;
	fi->f_pointer = file_get_pos(file_desc_from_idesc(idesc));

	assert(fi->f_buf);

	if (0 != (rc = ext2_read_inode(node, fi->f_num))) {
		ext2_close(node);
		return err2ptr(rc);
	}
	else {
		file_set_size(file_desc_from_idesc(idesc), fi->f_di.i_size);
	}

	return idesc;
}

static int ext2fs_close(struct file_desc *desc) {
	if (NULL == desc) {
		return 0;
	}

	return ext2_close(desc->f_inode);
}

static size_t ext2fs_read(struct file_desc *desc, void *buff, size_t size) {
	int rc;
	size_t csize;
	char *buf;
	size_t buf_size;
	char *addr = buff;
	struct inode *node;
	struct ext2_file_info *fi;

	node = desc->f_inode;
	fi = inode_priv(node);
	fi->f_pointer = file_get_pos(desc);

	while (size != 0) {
		/* XXX should handle LARGEFILE */
		if (fi->f_pointer >= (long) fi->f_di.i_size) {
			break;
		}

		if (0 != (rc = ext2_buf_read_file(node, &buf, &buf_size))) {
			SET_ERRNO(rc);
			return 0;
		}

		csize = size;
		if (csize > buf_size) {
			csize = buf_size;
		}

		memcpy(addr, buf, csize);

		fi->f_pointer += csize;
		addr += csize;
		size -= csize;
	}

	return (addr - (char *) buff);
}

static size_t ext2fs_write(struct file_desc *desc, void *buff, size_t size) {
	uint32_t bytecount;
	struct inode *node;
	struct ext2_file_info *fi;

	node = desc->f_inode;
	fi = inode_priv(node);
	fi->f_pointer = file_get_pos(desc);

	bytecount = ext2_write_file(node, buff, size);

	file_set_size(desc, fi->f_di.i_size);

	return bytecount;
}

struct file_operations ext2_fop = {
	.open = ext2fs_open,
	.close = ext2fs_close,
	.read = ext2fs_read,
	.write = ext2fs_write,
};
