/**
 * @file
 *
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */


#include <mem/objalloc.h>

#include <fs/file_desc.h>
#include <embox/unit.h>


OBJALLOC_DEF(desc_pool, struct file_desc, OPTION_GET(NUMBER,fdesc_quantity));

struct file_desc *file_desc_alloc(void) {
	return objalloc(&desc_pool);
}

void file_desc_free(struct file_desc *desc) {
	objfree(&desc_pool, desc);
}


struct file_desc *file_desc_create(struct node *node, int __oflag, mode_t imode) {
	//return file_desc_alloc();
	return NULL;
}

int file_desc_desctroy(struct file_desc *fdesc) {
	//fdesc->node-> --;
	file_desc_free(fdesc);
	return 0;
}

int file_desc_perm_check(struct file_desc *fdesc) {
	return 0;
}
