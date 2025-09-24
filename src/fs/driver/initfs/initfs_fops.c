/**
 * @
 *
 * @date Dec 23, 2019
 * @author Anton Bondarev
 */
#include <cpio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#include <fs/dir_context.h>
#include <fs/file_desc.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/initfs.h>

int initfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	return -EACCES;
}

static size_t initfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct initfs_file_info *fi;
	off_t pos;

	pos = file_get_pos(desc);
	fi = file_get_inode_data(desc);

	if (pos + size > file_get_size(desc)) {
		size = file_get_size(desc) - pos;
	}

	memcpy(buf, (char *) (uintptr_t) (fi->start_pos + pos), size);

	return size;
}

static int initfs_ioctl(struct file_desc *desc, int request, void *data) {
	char **p_addr;
	struct initfs_file_info *fi;

	fi = file_get_inode_data(desc);

	p_addr = data;

	*p_addr = (char*) (uintptr_t) fi->start_pos;

	return 0;
}

static size_t initfs_write(struct file_desc *desc, void *buf, size_t size) {
	return -EACCES;
}

struct file_operations initfs_fops = {
	.read  = initfs_read,
	.write = initfs_write,
	.ioctl = initfs_ioctl,
};

/**
* @brief Initialize initfs inode
*
* @param node  Structure to be initialized
* @param entry Information about file in cpio archieve
*
* @return Negative error code
*/
int initfs_fill_inode(struct inode *node, char *cpio,
		struct cpio_entry *entry) {
	struct initfs_file_info *fi;

	fi = inode_priv(node);
	assert(fi);

	inode_size_set(node, entry->size);
	inode_mtime_set(node, entry->mtime);
	node->i_mode = entry->mode & (S_IFMT | S_IRWXA);

	fi->start_pos = (uintptr_t) entry->data;
	fi->entry     = (void *) cpio;

	if (S_ISDIR(entry->mode)) {
		fi->path      = entry->name;
		fi->path_len  = strlen(entry->name);
	}

	return 0;
}

int initfs_fillname(struct inode *inode, char *buf) {
	struct cpio_entry entry;
	struct initfs_file_info *fi = inode_priv(inode);
	size_t name_len = 0;
	char *name;

	if (NULL == cpio_parse_entry((char *) fi->entry, &entry)) {
		return -1;
	}

	/* CPIO entry contains full path, so we need to throw away
	 * everything before the last '/' symbol */
	name = strrchr(entry.name, '/');
	if (name) {
		name++;
	} else {
		name = entry.name;
	}

	name_len = entry.name_len - (name - entry.name);

	memcpy(buf, name, name_len);
	buf[name_len] = '\0';

	return 0;
}

int initfs_destroy_inode(struct inode *inode) {
	if (inode_priv(inode) != NULL) {
		initfs_free_inode(inode_priv(inode));
	}

	return 0;
}

extern struct super_block_operations initfs_sbops;
extern struct inode_operations initfs_iops;

int initfs_fill_sb(struct super_block *sb, const char *source) {
	extern char _initfs_start, _initfs_end;
	struct initfs_file_info *fi;

	if (&_initfs_start == &_initfs_end) {
		return -1;
	}

	fi = initfs_alloc_inode();
	if (fi == NULL) {
		return -ENOMEM;
	}

	sb->sb_iops = &initfs_iops;
	sb->sb_fops = &initfs_fops;
	sb->sb_ops  = &initfs_sbops;
	sb->bdev    = NULL;

	memset(fi, 0, sizeof(struct initfs_file_info));
	inode_priv_set(sb->sb_root, fi);

	sb->sb_root->i_ops = &initfs_iops;

	return 0;
}
