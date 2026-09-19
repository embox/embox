/**
 * @file
 *
 * @date 17 Mar 2015
 * @author Denis Deryugin
 */
#include <sys/types.h>

#include <errno.h>

#include <fs/inode.h>
#include <fs/file_desc.h>
#include <util/atomic_rmw.h>

unsigned long fdesc_dead_inode;
unsigned long fdesc_stale_gen;

/* A descriptor pins its dentry and the dentry its inode, so a changed
 * generation means a lost pin. A dying inode is refused: the driver let go of
 * the file with its name. */
int dvfs_file_valid(struct file_desc *desc) {
	struct inode *inode = desc->f_inode;

	if (inode == NULL) {
		return 0;
	}
	if (inode->i_gen != desc->f_gen) {
		atomic_rmw_add_fetch(&fdesc_stale_gen, 1, __ATOMIC_RELAXED);
		return -EBADF;
	}
	if (inode->i_dying) {
		atomic_rmw_add_fetch(&fdesc_dead_inode, 1, __ATOMIC_RELAXED);
		return -EBADF;
	}
	return 0;
}

off_t file_get_pos(struct file_desc *file) {
	return file->f_pos;
}

off_t file_set_pos(struct file_desc *file, off_t off) {
	file->f_pos = off;
	return file->f_pos;
}

size_t file_get_size(struct file_desc *file) {
	return file->f_inode->i_size;
}

void file_set_size(struct file_desc *file, size_t size) {
	file->f_inode->i_size = size;
}

void *file_get_inode_data(struct file_desc *file) {
	assert(file);
	assert(file->f_inode);

	return file->f_inode->i_privdata;
}

struct file_desc *file_desc_from_idesc(struct idesc *idesc) {
	return (struct file_desc *)idesc;
}
