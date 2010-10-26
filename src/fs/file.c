/**
 * @file
 *
 * @date 06.08.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <lib/list.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <embox/kernel.h>

typedef struct lsof_map {
	struct list_head *next;
	struct list_head *prev;
	const char        path[CONFIG_MAX_LENGTH_FILE_NAME];
	FILE             *fd;
} lsof_map_t;

static lsof_map_t lsof_pool[CONFIG_QUANTITY_NODE];
static LIST_HEAD(free_list);
static LIST_HEAD(fd_cache);

#define fd_to_head(file) (uint32_t)(file - offsetof(lsof_map_t, fd))

void lsof_map_init(void) {
        size_t i;
        for (i = 0; i < ARRAY_SIZE(lsof_pool); i++) {
                list_add((struct list_head *) &lsof_pool[i], &free_list);
        }
}

static void cache_fd(const char *path, FILE *file) {
	lsof_map_t *head;
	if(list_empty(&free_list)) {
		return;
	}
	head = (lsof_map_t *)free_list.next;
	head->fd = file;
	strcpy((void*)head->path, path);
	list_move((struct list_head*)head, &fd_cache);
}

static void uncache_fd(FILE *file) {
	list_move((struct list_head*)fd_to_head(file), &free_list);
}

static lsof_map_t *find_fd(FILE *file) {
	struct list_head *p;
	list_for_each(p, &fd_cache) {
		if(((lsof_map_t *)p)->fd == file) {
			return (lsof_map_t *)p;
		}
	}
	TRACE("File maybe not opened\n");
	return NULL;
}

FILE *fopen(const char *path, const char *mode) {
	node_t *nod = vfs_find_node(path, NULL);
	file_system_driver_t *drv;
	FILE *file;
	if (nod == NULL) {
		//FIXME: ahtung! workaround.
		ramfs_create_param_t param;
		strcpy(param.name, path);
		param.size = 0;
		param.mode = 0;
		param.start_addr = 0x44100000;
		drv = find_filesystem("ramfs");
		drv->fsop->create_file(&param);
		nod = vfs_find_node(path, NULL);
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fopen) {
		LOG_ERROR("fop->fopen is NULL handler\n");
		return NULL;
	}
	file = drv->file_op->fopen(path, mode);
	cache_fd(path, file);
	return file;
}

FILE *fdopen(int fd, const char *mode) {
	return NULL;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	lsof_map_t *lsof;
	node_t *nod;
	file_system_driver_t *drv;

	lsof = find_fd(file);
	nod = vfs_find_node(lsof->path, NULL);
	if (NULL == nod) {
		LOG_ERROR("node is NULL\n");
		return -1;
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fwrite) {
		LOG_ERROR("fop->fwrite is NULL handler\n");
		return -1;
	}
	return drv->file_op->fwrite(buf, size, count, file);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	lsof_map_t *lsof;
	node_t *nod;
	file_system_driver_t *drv;
	lsof = find_fd(file);
	nod = vfs_find_node(lsof->path, NULL);
	if (NULL == nod){
		LOG_ERROR("node is NULL\n");
		return -1;
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fread) {
		LOG_ERROR("fop->fread is NULL handler\n");
		return -1;
	}
	return drv->file_op->fread(buf, size, count, file);
}

int fclose(FILE *fp) {
	lsof_map_t *lsof;
	node_t *nod;
	file_system_driver_t *drv;
	lsof = find_fd(fp);
	nod = vfs_find_node(lsof->path, NULL);
	if (NULL == nod) {
		return EOF;
	}
	drv = nod->fs_type;
	if (NULL == drv->file_op->fclose) {
		return EOF;
	}
	uncache_fd(fp);
	return drv->file_op->fclose(fp);
}

int fseek(FILE * stream, long int offset, int origin) {
	file_operations_t **fop = (file_operations_t **)stream;

	if (NULL == fop) {
		LOG_ERROR("fop is NULL handler\n");
		return -1;
	}

	if (NULL == (*fop)->fseek){
		LOG_ERROR("fop->fseek is NULL handler\n");
		return -1;
	}

	return (*fop)->fseek(stream, offset, origin);
}
