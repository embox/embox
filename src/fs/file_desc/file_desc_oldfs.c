/**
 * @file
 *
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>

#include <fs/perm.h>
#include <mem/objalloc.h>

#include <fs/file_desc.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <embox/unit.h>
#include <util/err.h>
#include <fs/inode.h>

extern const struct idesc_xattrops file_idesc_xattrops;

OBJALLOC_DEF(desc_pool, struct file_desc, OPTION_GET(NUMBER,fdesc_quantity));

static struct file_desc *file_desc_alloc(void) {
	return objalloc(&desc_pool);
}

static void file_desc_free(struct file_desc *desc) {
	objfree(&desc_pool, desc);
}

extern const struct idesc_ops idesc_file_ops;

struct file_desc *file_desc_create(struct inode *node, int flag) {
	struct file_desc *desc;
	int perm_flags;
	int ret;

	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc())) {
		return err_ptr(ENOMEM);
	}
	/* setup access mode */
	perm_flags = 0;
	if ((flag & O_WRONLY) || (flag & O_RDWR)) {
		perm_flags |= S_IWOTH;
	}
	if (!(flag & O_WRONLY)) {
		perm_flags |= S_IROTH;
	}

	if (0 > (ret = fs_perm_check(node, perm_flags))) {
		file_desc_free(desc);
		return err_ptr(EACCES);
	}

	desc->f_inode = node;
	desc->file_flags = flag & O_APPEND;
	desc->f_pos = 0;

	idesc_init(&desc->f_idesc, &idesc_file_ops, flag);
	desc->f_idesc.idesc_xattrops = &file_idesc_xattrops;

	return desc;
}

int file_desc_destroy(struct file_desc *fdesc) {
	assert(fdesc);

	inode_del(fdesc->f_inode);

	file_desc_free(fdesc);
	return 0;
}

struct file_desc *file_desc_get(int idx) {
	struct idesc *idesc;
	struct idesc_table *it;

	assert(idesc_index_valid(idx));

	it = task_resource_idesc_table(task_self());
	assert(it);

	idesc = idesc_table_get(it, idx);
	assert(idesc);

	if (idesc->idesc_ops != &idesc_file_ops) {
		return NULL;
	}

	return (struct file_desc *) idesc;
}

off_t file_get_pos(struct file_desc * file) {
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
	assert(file->f_inode);

	return file->f_inode->i_privdata;
}

struct file_desc *file_desc_from_idesc(struct idesc *idesc) {
	return (struct file_desc *)idesc;
}

void file_desc_set_file_info(struct file_desc *file, void *fi) {
	file->file_info = fi;
}
