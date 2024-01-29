/**
 * @file
 *
 * @date 17 Mar 2015
 * @author Denis Deryugin
 */
#include <sys/types.h>

#include <fs/dvfs.h>
#include <fs/file_desc.h>

off_t file_get_pos(struct file_desc *file) {
	return file->pos;
}

off_t file_set_pos(struct file_desc *file, off_t off) {
	file->pos = off;
	return file->pos;
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

	return file->f_inode->i_data;
}

struct file_desc *file_desc_from_idesc(struct idesc *idesc) {
	return (struct file_desc *)idesc;
}
