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

#include <fs/perm.h>
#include <fs/flags.h>
#include <mem/objalloc.h>

#include <fs/file_desc.h>
#include <embox/unit.h>
#include <err.h>


OBJALLOC_DEF(desc_pool, struct file_desc, OPTION_GET(NUMBER,fdesc_quantity));

static struct file_desc *file_desc_alloc(void) {
	return objalloc(&desc_pool);
}

static void file_desc_free(struct file_desc *desc) {
	objfree(&desc_pool, desc);
}


struct file_desc *file_desc_create(struct node *node, int flag) {
	struct file_desc *desc;
	int perm_flags;
	int ret;

	perm_flags = ((flag & O_WRONLY || flag & O_RDWR) ? FS_MAY_WRITE : 0)
		| ((flag & O_WRONLY) ? 0 : FS_MAY_READ);

	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc())) {
		return err_ptr(ENOMEM);
	}

	desc->node = node;

	desc->flags = perm_flags | ((flag & O_APPEND) ? FS_MAY_APPEND : 0);
	desc->cursor = 0;
	io_sync_init(&desc->ios, 0, 0);


	if (0 > (ret = fs_perm_check(node, perm_flags))) {
		file_desc_free(desc);
		return err_ptr(EACCES);
	}

	return desc;
}

int file_desc_destroy(struct file_desc *fdesc) {
	file_desc_free(fdesc);
	return 0;
}

int file_desc_perm_check(struct file_desc *fdesc) {
	return 0;
}
