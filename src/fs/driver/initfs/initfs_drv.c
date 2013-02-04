/**
 * @file
 * @details realize file operation function in line address space
 *
 * @date 29.06.09
 * @author Anton Bondarev
 *	- initial implementation
 * @author Nikolay Korotky
 *	- rework using vfs
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <fs/ramfs.h>
#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <fs/file_desc.h>

#include <mem/misc/pool.h>
#include <util/array.h>
#include <embox/unit.h>


typedef struct ramfs_file_info {
	struct node_info ni; /* it must be first member */
    unsigned long start_addr;
}ramfs_file_info_t;

/* file description pool */
POOL_DEF (fdesc_pool, struct ramfs_file_info,
			OPTION_GET(NUMBER,fdesc_quantity));

/* File operations */

static int    ramfs_open(struct node *node, struct file_desc *desk, int flags);
static int    ramfs_close(struct file_desc *desc);
static size_t ramfs_read(struct file_desc *desc, void *buf, size_t size);
static int    ramfs_ioctl(struct file_desc *desc, int request, va_list args);

static struct kfile_operations ramfs_fop = { ramfs_open, ramfs_close, ramfs_read,
		NULL,  ramfs_ioctl };

static int ramfs_open(struct node *nod, struct file_desc *desc, int flags) {
	return 0;
}

static int ramfs_close(struct file_desc *desc) {
	return 0;
}

static size_t ramfs_read(struct file_desc *desc, void *buf, size_t size) {
	ramfs_file_info_t *fi;
	struct nas *nas;

	nas = desc->node->nas;
	fi = (ramfs_file_info_t*) nas->fi;

	if (fi == NULL) {
		return -ENOENT;
	}

	if (size > (fi->ni.size - desc->cursor)) {
		size = fi->ni.size - desc->cursor;
	}

	memcpy((void*) buf, (const void *) (fi->start_addr + desc->cursor),
			size);
	desc->cursor += size;

	return size;
}

static int ramfs_ioctl(struct file_desc *desc, int request, va_list arg) {
	ramfs_file_info_t *fi;
	uint32_t *addr;
	va_list args;
	struct nas *nas;
	//TODO: switch through "request" ID.
	va_copy(args, arg);
	addr = (uint32_t *) va_arg(args, unsigned long);
	va_end(args);
	nas = desc->node->nas;
	fi = (ramfs_file_info_t*) nas->fi;
	*addr = fi->start_addr;
	return 0;
}

/* File system operations */

static int ramfs_mount(void * dev, void *dir);
static int ramfs_create(struct node *parent_node, struct node *new_nodes);

static fsop_desc_t ramfs_fsop = { NULL, NULL, ramfs_mount,
		ramfs_create, NULL };

static fs_drv_t ramfs_drv = { "ramfs", &ramfs_fop, &ramfs_fsop };

DECLARE_FILE_SYSTEM_DRIVER(ramfs_drv);

static int ramfs_create(struct node *parent_node, struct node *new_node) {
	ramfs_create_param_t *par;
	struct nas *nas, *root_nas;
	ramfs_file_info_t *fi;


	nas = new_node->nas;
	par = (ramfs_create_param_t *) nas->fi;

	fi = pool_alloc(&fdesc_pool);


	root_nas = par->root_node->nas;
	nas->fs = root_nas->fs;
	nas->fi = (void *) fi;

	fi->start_addr = par->start_addr;
	fi->ni.size = par->size;
	fi->ni.mtime = par->mtime;

	new_node->mode = par->mode;

	return 0;
}


static int ramfs_mount(void * dev, void *dir) {
	node_t *dir_node = dir;
	assert(dir_node);
	return cpio_unpack(dir_node);
}
