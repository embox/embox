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


#include <fs/perm.h>
#include <fs/flags.h>
#include <mem/objalloc.h>

#include <fs/file_desc.h>
#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <kernel/task/resource/idesc_table.h>
#include <embox/unit.h>
#include <util/err.h>

extern const struct idesc_xattrops file_idesc_xattrops;

OBJALLOC_DEF(desc_pool, struct file_desc, OPTION_GET(NUMBER,fdesc_quantity));

static struct file_desc *file_desc_alloc(void) {
	return objalloc(&desc_pool);
}

static void file_desc_free(struct file_desc *desc) {
	objfree(&desc_pool, desc);
}

extern const struct idesc_ops idesc_file_ops;

struct file_desc *file_desc_create(struct node *node, int flag) {
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
		perm_flags |= FS_MAY_WRITE;
	}
	if (!(flag & O_WRONLY)) {
		perm_flags |= FS_MAY_READ;
	}

	if (0 > (ret = fs_perm_check(node, perm_flags))) {
		file_desc_free(desc);
		return err_ptr(EACCES);
	}

	desc->node = node;
	desc->file_flags = flag & O_APPEND;
	desc->cursor = 0;

	idesc_init(&desc->idesc, &idesc_file_ops, perm_flags);
	desc->idesc.idesc_xattrops = &file_idesc_xattrops;

	return desc;
}

int file_desc_destroy(struct file_desc *fdesc) {
	assert(fdesc);
	assert(fdesc->idesc.idesc_ops == &idesc_file_ops);

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
