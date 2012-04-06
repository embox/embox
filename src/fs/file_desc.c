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
