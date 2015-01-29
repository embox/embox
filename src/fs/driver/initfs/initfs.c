/**
 * @file
 * @details Read-only filesystem with direct address space mapping.
 *
 * @date 29.06.09
 * @author Anton Bondarev
 *	        - initial implementation
 * @author Nikolay Korotky
 *	        - rework using vfs
 * @author Eldar Abusalimov
 *	        - rework mount to use cpio_parse_entry
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <cpio.h>
#include <stdarg.h>
#include <limits.h>

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

#include <mem/misc/pool.h>
#include <kernel/printk.h>
#include <util/array.h>
#include <embox/unit.h>


struct initfs_file_info {
	struct node_info ni; /* must be the first member */
    char *addr;
};

POOL_DEF (fdesc_pool, struct initfs_file_info,
		OPTION_GET(NUMBER,fdesc_quantity));

static int initfs_open(struct node *nod, struct file_desc *desc, int flags) {
	return 0;
}

static int initfs_close(struct file_desc *desc) {
	return 0;
}

static size_t initfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct initfs_file_info *fi;
	struct nas *nas;

	nas = desc->node->nas;
	fi = (struct initfs_file_info*) nas->fi;

	if (!fi) {
		return -ENOENT;
	}

	if (size > fi->ni.size - desc->cursor) {
		size = fi->ni.size - desc->cursor;
	}

	memcpy(buf, fi->addr + desc->cursor, size);
	desc->cursor += size;

	return size;
}

static int initfs_ioctl(struct file_desc *desc, int request, ...) {
	struct nas *nas;
	struct initfs_file_info *fi;
	char **p_addr;
	va_list args;

	//TODO: switch through "request" ID.
	//va_copy(args, arg);
	va_start(args, request);
	p_addr = va_arg(args, char **);
	va_end(args);

	nas = desc->node->nas;
	fi = (struct initfs_file_info *) nas->fi;
	assert(p_addr != NULL);
	*p_addr = fi->addr;

	return 0;
}

static int initfs_mount(void *dev, void *dir) {
	extern char _initfs_start, _initfs_end;
	char *cpio = &_initfs_start;
	struct nas *dir_nas;
	struct node *node;
	struct initfs_file_info *fi;
	struct cpio_entry entry;
	char name[PATH_MAX + 1];

	struct node *dir_node = dir;

	dir_nas = dir_node->nas;
	dir_nas->fs = filesystem_create("initfs");

	if (&_initfs_start == &_initfs_end) {
		return -1;
	}
	printk("%s: unpack initinitfs at %p into %s\n", __func__,
			cpio, dir_node->name);

	while ((cpio = cpio_parse_entry(cpio, &entry))) {
		if (entry.name_len > PATH_MAX) {
			return -ENAMETOOLONG;
		}
		memcpy(name, entry.name, entry.name_len);
		name[entry.name_len] = '\0';

		if (NULL == (node =
				vfs_subtree_create_intermediate(dir_node, name, entry.mode))) {
			return -ENOMEM;
		}

		fi = pool_alloc(&fdesc_pool);
		if (!fi) {
			return -ENOMEM;
		}

		fi->addr = entry.data;
		fi->ni.size = entry.size;
		fi->ni.mtime = entry.mtime;

		node->nas->fi = (struct node_fi *) fi;
		node->nas->fs = dir_nas->fs;
	}

	return 0;
}

static struct kfile_operations initfs_fop = {
	.open = initfs_open,
	.close = initfs_close,
	.read = initfs_read,
	.ioctl = initfs_ioctl,
};

static struct fsop_desc initfs_fsop = {
	.mount = initfs_mount,
};

static struct fs_driver initfs_driver = {
	.name = "initfs",
	.file_op = &initfs_fop,
	.fsop = &initfs_fsop,
	.mount_dev_by_string = true,
};

DECLARE_FILE_SYSTEM_DRIVER(initfs_driver);
