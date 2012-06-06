/**
 * @file
 * @brief
 *
 * @date 12.05.2012
 * @author Andrey Gazukin
 */


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <fs/vfs.h>
#include <fs/ramdisk.h>
#include <fs/fs_drv.h>
#include <fs/node.h>
#include <fs/nfs.h>
#include <fs/nfs_clientstate.h>
#include <util/array.h>
#include <embox/device.h>
#include <mem/page.h>
#include <cmd/mount.h>

//static uint8_t sector_buff[SECTOR_SIZE];
//static uint32_t bytecount;

/* nfs filesystem description pool */

typedef struct nfs_fs_description_head {
	struct list_head *next;
	struct list_head *prev;
	nfs_fs_description_t desc;
} nfs_fs_description_head_t;

static nfs_fs_description_head_t nfs_fs_pool[QUANTITY_RAMDISK];
static LIST_HEAD(nfs_free_fs);

#define param_to_head_fs(fs_param) \
	(uint32_t)(fs_param - offsetof(nfs_fs_description_head_t, desc))

static void init_nfs_fsinfo_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(nfs_fs_pool); i++) {
		list_add((struct list_head *) &nfs_fs_pool[i], &nfs_free_fs);
	}
}

static nfs_fs_description_t *nfs_fsinfo_alloc(void) {
	nfs_fs_description_head_t *head;
	nfs_fs_description_t *desc;

	if (list_empty(&nfs_free_fs)) {
		return NULL;
	}
	head = (nfs_fs_description_head_t *) (&nfs_free_fs)->next;
	list_del((&nfs_free_fs)->next);
	desc = &(head->desc);
	return desc;
}

static void nfs_fsinfo_free(nfs_fs_description_t *desc) {
	if (NULL == desc) {
		return;
	}
	list_add((struct list_head*) param_to_head_fs(desc), &nfs_free_fs);
}

/* nfs file description pool */

typedef struct nfs_file_description_head {
	struct list_head *next;
	struct list_head *prev;
	nfs_file_description_t desc;
} nfs_file_description_head_t;

static nfs_file_description_head_t nfs_files_pool[MAX_FILE_QUANTITY];
static LIST_HEAD(nfs_free_file);

#define param_to_head_file(file_param) \
	(uint32_t)(file_param - offsetof(nfs_file_description_head_t, desc))

static void init_nfs_fileinfo_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(nfs_files_pool); i++) {
		list_add((struct list_head *) &nfs_files_pool[i], &nfs_free_file);
	}
}

static nfs_file_description_t *nfs_fileinfo_alloc(void) {
	nfs_file_description_head_t *head;
	nfs_file_description_t *desc;

	if (list_empty(&nfs_free_file)) {
		return NULL;
	}
	head = (nfs_file_description_head_t *) (&nfs_free_file)->next;
	list_del((&nfs_free_file)->next);
	desc = &(head->desc);
	return desc;
}

static void nfs_fileinfo_free(nfs_file_description_t *desc) {
	if (NULL == desc) {
		return;
	}
	list_add((struct list_head*) param_to_head_file(desc), &nfs_free_file);
}

/* File operations */

static void *nfsfs_fopen(struct file_desc *desc,  const char *mode);
static int nfsfs_fclose(struct file_desc *desc);
static size_t nfsfs_fread(void *buf, size_t size, size_t count, void *file);
static size_t nfsfs_fwrite(const void *buf, size_t size, size_t count,
		void *file);
static int nfsfs_fseek(void *file, long offset, int whence);
static int nfsfs_ioctl(void *file, int request, va_list args);

static file_operations_t nfsfs_fop = { nfsfs_fopen, nfsfs_fclose, nfsfs_fread,
		nfsfs_fwrite, nfsfs_fseek, nfsfs_ioctl };
/*
 * file_operation
 */
static void *nfsfs_fopen(struct file_desc *desc, const char *mode) {
	node_t *nod;
	uint8_t _mode;
	//char path [CONFIG_MAX_LENGTH_PATH_NAME];
	nfs_file_description_t *fd;

	nod = desc->node;
	fd = (nfs_file_description_t *)nod->attr;

	if ('r' == *mode) {
		_mode = O_RDONLY;
	}
	else if ('w' == *mode) {
		_mode = O_WRONLY;
	}
	else {
		_mode = O_RDONLY;
	}

	/*nfsfs_set_path ((uint8_t *) path, nod);
	cut_mount_dir((char *) path, fd->p_fs_dsc->root_name);

	if(0 == nfs_open_file(fd, (uint8_t *)path, _mode, sector_buff)) {
		return desc;
	}*/
	return NULL;
}

static int nfsfs_fseek(void *file, long offset, int whence) {
	struct file_desc *desc;
	nfs_file_description_t *fd;
	uint32_t curr_offset;

	curr_offset = offset;

	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->attr;

	switch (whence) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		//curr_offset += fd->fi.pointer;
		break;
	case SEEK_END:
		//curr_offset = fd->fi.filelen;
		break;
	default:
		return -1;
	}

	/*nfs_fseek(fd, curr_offset, sector_buff);*/
	return 0;
}

static int nfsfs_fclose(struct file_desc *desc) {
	return 0;
}

static size_t nfsfs_fread(void *buf, size_t size, size_t count, void *file) {
	size_t size_to_read;
	struct file_desc *desc;
	size_t rezult;
	nfs_file_description_t *fd;

	size_to_read = size * count;
	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->attr;

	rezult = 0; /*nfs_read_file(fd, sector_buff, buf, &bytecount, size_to_read);
	if (0 == rezult) {
		return bytecount;
	}*/
	return rezult;
}

static size_t nfsfs_fwrite(const void *buf, size_t size,
	size_t count, void *file) {
	size_t size_to_write;
	struct file_desc *desc;
	size_t rezult;
	nfs_file_description_t *fd;

	size_to_write = size * count;
	desc = (struct file_desc *) file;

	fd = (nfs_file_description_t *)desc->node->attr;

	rezult = 0;/*nfs_write_file(fd, sector_buff, (uint8_t *)buf,
			&bytecount, size_to_write);
	if (0 == rezult) {
		return bytecount;
	}*/
	return rezult;
}

static int nfsfs_ioctl(void *file, int request, va_list args) {
	return 0;
}

/* File system operations */

static int nfsfs_init(void * par);
static int nfsfs_format(void * par);
static int nfsfs_mount(void * par);
static int nfsfs_create(void *par);
static int nfsfs_delete(const char *fname);

static fsop_desc_t nfsfs_fsop = { nfsfs_init, nfsfs_format, nfsfs_mount,
		nfsfs_create, nfsfs_delete };

static fs_drv_t nfsfs_drv = { "nfs", &nfsfs_fop, &nfsfs_fsop };

static int nfsfs_init(void * par) {
	init_nfs_fsinfo_pool();
	init_nfs_fileinfo_pool();
	return 0;
}

static int nfsfs_format(void *par) {
	ramdisk_params_t *params;
	node_t *nod;

	params = (ramdisk_params_t *) par;
	if (NULL == (nod = vfs_find_node(params->path, NULL))) {
		return -ENODEV;/*device not found*/
	}

	return 0;
}

static int nfsfs_mount(void *par) {
	mount_params_t *params;
	node_t *dir_node;
	nfs_fs_description_t *fs_des;
	nfs_file_description_t *fd;
	int rezult;

	params = (mount_params_t *) par;

	if (NULL == (dir_node = vfs_find_node(params->dir, NULL))) {
		/*TODO usually mount doesn't create a directory*/
		if (NULL == (dir_node = vfs_add_path (params->dir, NULL))) {
			return -ENODEV;/*device not found*/
		}
		dir_node->properties = DIRECTORY_NODE_TYPE;
	}

	fs_des = nfs_fsinfo_alloc();
	fd = nfs_fileinfo_alloc();
	fd->p_fs_dsc = fs_des;

	dir_node->fs_type = &nfsfs_drv;
	dir_node->file_info = (void *) &nfsfs_fop;
	dir_node->attr = (void *) fd;
	params->dev_node = dir_node;

	rezult = mount_nfs_filesystem(params);
	if(0 == rezult) {
		memcpy(&fd->fh, &fd->p_fs_dsc->fh, sizeof(fd->fh));
	}

	return rezult;
}

static int nfsfs_create(void *par) {
	file_create_param_t *param;
	nfs_file_description_t *fd;
	node_t *node, *parents_node;
	int node_quantity;

	param = (file_create_param_t *) par;

	node = (node_t *)param->node;
	parents_node = (node_t *)param->parents_node;

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
		node_quantity = 3; /* need create . and .. directory */
	}
	else {
		node_quantity = 1;
	}

	for (int count = 0; count < node_quantity; count ++) {
		if(0 < count) {
			if(1 == count) {
				strcat(param->path, "/.");
			}
			else if(2 == count) {
				strcat(param->path, ".");
			}
			node = vfs_add_path (param->path, NULL);
		}

		fd = nfs_fileinfo_alloc();
		fd->p_fs_dsc = ((nfs_file_description_t *)
				parents_node->attr)->p_fs_dsc;
		node->fs_type = &nfsfs_drv;
		node->file_info = (void *) &nfsfs_fop;
		node->attr = (void *)fd;

		/*
		 * nfsfs_create_file called only once for the newly created directory.
		 * Creation of dir . and .. occurs into the function nfsfs_create_file.
		 */
		/*if(0 >= count) {
			nfsfs_create_file(par);
		}*/
	}
	/* cut /.. from end of PATH, if need */
	if (1 < node_quantity) {
		param->path[strlen(param->path) - 3] = '\0';
	}

	return 0;
}

static int nfsfs_delete(const char *fname) {
	nfs_file_description_t *fd;
	node_t *nod;
	//char path [CONFIG_MAX_LENGTH_PATH_NAME];

	nod = vfs_find_node(fname, NULL);
	fd = nod->attr;

	/*nfsfs_set_path ((uint8_t *) path, nod);
	cut_mount_dir((char *) path, fd->p_fs_dsc->root_name);

	if(nfs_unlike_file(fd, (uint8_t *) path, (uint8_t *) sector_buff)) {
		return -1;
	}*/

	nfs_fileinfo_free(fd);
	if(0)nfs_fsinfo_free(fd->p_fs_dsc);
	vfs_del_leaf(nod);
	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(nfsfs_drv);

