/**
 * @file
 * @details realize file operation function in line address space
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 *	- initial implementation
 * @author Nikolay Korotky
 *	- rework using vfs
 */
#include <string.h>
#include <fs/ramfs.h>
#include <fs/fs.h>
#include <fs/vfs.h>
#include <linux/init.h>
#include <embox/kernel.h>

typedef struct ramfs_file_description_head {
        struct list_head         *next;
        struct list_head         *prev;
        ramfs_file_description_t  desc;
} ramfs_file_description_head_t;

static ramfs_file_description_head_t fdesc_pool[CONFIG_QUANTITY_NODE];
static LIST_HEAD(fdesc_free);

#define desc_to_head(fdesc) (uint32_t)(fdesc - offsetof(ramfs_file_description_head_t, desc))

static void init_ramfs_info_pool(void) {
        size_t i;
        for(i = 0; i < ARRAY_SIZE(fdesc_pool); i ++) {
                list_add((struct list_head *)&fdesc_pool[i], &fdesc_free);
        }
}

static ramfs_file_description_t *ramfs_info_alloc(void) {
        ramfs_file_description_head_t *head;
        ramfs_file_description_t *desc;

        if(list_empty(&fdesc_free)) {
                return NULL;
        }
        head = (ramfs_file_description_head_t *)(&fdesc_free)->next;
        list_del((&fdesc_free)->next);
        desc = &(head->desc);
        return desc;
}

static void ramfs_info_free(ramfs_file_description_t *desc) {
	if (NULL == desc) {
		return;
	}
        list_add((struct list_head*)desc_to_head(desc), &fdesc_free);
}

/* File operations */

static void   *ramfs_fopen(const char *path, const char *mode);
static int     ramfs_fclose(void *file);
static size_t  ramfs_fread(void *buf, size_t size, size_t count, void *file);
static size_t  ramfs_fwrite(const void *buf, size_t size, size_t count, void *file);
static int     ramfs_fseek(void *file, long offset, int whence);

static file_operations_t ramfs_fop = {
	ramfs_fopen,
	ramfs_fclose,
	ramfs_fread,
	ramfs_fwrite,
	ramfs_fseek
};

static void *ramfs_fopen(const char *file_name, const char *mode) {
	node_t *nod;
	ramfs_file_description_t *fd;

	if (NULL == (nod = vfs_find_node(file_name, NULL))) {
		TRACE("can't find file %s\n", file_name);
		return NULL;
	}
	fd = (ramfs_file_description_t*)nod->file_info;
	fd->cur_pointer = 0;
	fd->lock = 1;
	return fd;
}

static int ramfs_fclose(void *file) {
	ramfs_file_description_t *fd;
	fd = (ramfs_file_description_t*)file;
	fd->lock = 0;
	return 0;
}

static size_t ramfs_fread(void *buf, size_t size, size_t count, void *file) {
	ramfs_file_description_t *fd;
	fd = (ramfs_file_description_t*)file;
	size_t size_to_read = size*count;

	if (fd == NULL) {
		return -2; /*Null file descriptor*/
	}

	if (size*count >= (fd->size - fd->cur_pointer)) {
		size_to_read = fd->size - fd->cur_pointer;
	}

	memcpy((void*)buf, (const void *)(fd->start_addr + fd->cur_pointer), size_to_read);
	fd->cur_pointer += size_to_read;
	return size_to_read;
}

static size_t ramfs_fwrite(const void *buf, size_t size, size_t count, void *file) {
	ramfs_file_description_t *fd;
	fd = (ramfs_file_description_t*)file;
	size_t size_to_write = size*count;

	if (fd == NULL) {
		return -2; /*Null file descriptor*/
	}

	//FIXME: don't expand memory, need file ramfs_resize.
	if (size*count >= (fd->size - fd->cur_pointer)) {
		fd->size += size*count;
		//size_to_write = fd->size - fd->cur_pointer;
	}

	memcpy((void *)(fd->start_addr + fd->cur_pointer), buf, size_to_write);
	fd->cur_pointer += size_to_write;
	return size_to_write;
}

static int ramfs_fseek(void *file, long offset, int whence) {
	ramfs_file_description_t *fd;
	fd = (ramfs_file_description_t*)file;
	int new_offset;

	if (fd == NULL) {
		return -2; /*Null file descriptor*/
	}

	switch(whence) {
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset = offset + fd->cur_pointer;
		break;
	case SEEK_END:
		new_offset = fd->size - offset;
		break;
	default:
		new_offset = offset + whence;
	}

	if (new_offset >= fd->size) {
		return -1; /*Non-valid offset*/
	}

	fd->cur_pointer = new_offset;

        return 0;
}

/* File system operations */

static int ramfs_create(void *params);
static int ramfs_delete(const char *fname);
static int ramfs_init(void * par);

static fsop_desc_t ramfs_fsop = {
	ramfs_init,
	ramfs_create,
	ramfs_delete
};

static file_system_driver_t ramfs_drv = {
	"ramfs",
	&ramfs_fop,
	&ramfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(ramfs_drv);

static int ramfs_create(void *params) {
	ramfs_create_param_t *par;
	node_t *nod;
	ramfs_file_description_t *fd;

	par = (ramfs_create_param_t *)params;
	nod = vfs_add_path(par->name, NULL);
	fd  = ramfs_info_alloc();
	nod->fs_type   = &ramfs_drv;
	nod->file_info = (void *)fd;

	fd->start_addr = par->start_addr;
	fd->size       = par->size;
	fd->mode       = par->mode;
	fd->mtime      = par->mtime;

	return 0;
}

static int ramfs_delete(const char *fname) {
	ramfs_file_description_t *fd;
	node_t *nod = vfs_find_node(fname, NULL);
	fd = nod->file_info;

	ramfs_info_free(fd);
	vfs_del_leaf(nod);
	return 0;
}

static int __init ramfs_init(void * par) {
	init_ramfs_info_pool();
	TRACE("RAMFS: inited\n");
	return 0;
}
