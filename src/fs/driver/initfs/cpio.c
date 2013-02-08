/**
 * @file
 * @brief parse cpio initramfs
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <fs/fs_drv.h>
#include <cpio.h>


/**
 * The pathname is followed by NUL bytes so that the total size of the fixed
 * header plus pathname is a multiple of four. Likewise, the file data is
 * padded to a multiple of four bytes.
 */
#define N_ALIGN(len) ((((len) + 1) & ~3) + 2)
#define F_ALIGN(len) (((len) + 3) & ~3)


static cpio_newc_header_t *cpio_parse_item(cpio_newc_header_t *cpio_h, char *name, ramfs_create_param_t *param) {
	char *s;
	size_t i;
	unsigned long parsed[12], file_size, start_addr, mode, mtime;
	char buf[9];

	buf[8] = '\0';
	if (memcmp(cpio_h->c_magic, MAGIC_OLD_BINARY, 6)==0) {
		printk("Use -H newc option for create cpio arch\n");
		return NULL;
	}
	if (memcmp(cpio_h->c_magic, MAGIC_NEWC, 6)) {
		printk("Newc ASCII CPIO format not recognized.\n");
		return NULL;
	}
	s = (char*) cpio_h;
	for (i = 0, s += 6; i < 12; i++, s += 8) {
		memcpy(buf, s, 8);
		parsed[i] = strtol(buf, NULL, 16);
	}

	strncpy(name, (char*) cpio_h + sizeof(cpio_newc_header_t), parsed[11]);
	name[parsed[11]] = '\0';
	file_size  = parsed[6];
	start_addr = (unsigned long)cpio_h + sizeof(cpio_newc_header_t) + N_ALIGN(parsed[11]);
	mode       = parsed[1];
	mtime      = parsed[5];

	if (0 == strcmp(name, "TRAILER!!!")) {
		return NULL;
	} else {
		//TODO: set start_addr, file_size, mode.
		strncpy(param->name, name, parsed[11]);
		param->size = file_size;
		param->mode = mode;
		param->mtime = mtime;
		param->start_addr = start_addr;

	}
	return (cpio_newc_header_t*) F_ALIGN(start_addr + file_size);
}

int cpio_unpack(node_t *dir_node) {
	extern char _ramfs_start, _ramfs_end;
	cpio_newc_header_t *cpio_h, *cpio_next;
	char buff_name[MAX_LENGTH_FILE_NAME];
	struct nas *nas;
	struct node *node;
	ramfs_create_param_t param;

	if (&_ramfs_end == &_ramfs_start) {
		return -1;
	}
	printk("%s: initramfs at %p into %s\n", __func__, &_ramfs_start,
			dir_node->name);

	nas = dir_node->nas;
	nas->fs = alloc_filesystem("ramfs");

	param.root_node = dir_node;
	cpio_h = (cpio_newc_header_t *) &_ramfs_start;
	while (NULL != (cpio_next = cpio_parse_item(cpio_h, buff_name, &param))) {
		cpio_h = cpio_next;
		if (param.size == 0) {
			/* this is a directory */
			//printk("cpio initramfs not support directory now\n");
			if (NULL == (node = vfs_lookup(dir_node, param.name))) {
				return 0;/*file already exist*/
			}
			node->type = NODE_TYPE_DIRECTORY | S_IREAD; /* read only file */
			((struct nas *)(node->nas))->fi =(void *) &param;
			nas->fs->drv->fsop->create_node(dir_node, node);

		} else {
			/* this is a regular file */
			node = vfs_create_child(dir_node, param.name, S_IFREG | S_IRUSR);
			if (!node) {
				return 0; /* file already exists */
			}
			((struct nas *) (node->nas))->fi = (void *) &param;
			nas->fs->drv->fsop->create_node(dir_node, node);
		}
	}

	return 0;
}
