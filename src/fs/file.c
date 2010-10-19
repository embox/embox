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
		return NULL;
	}
	head = (lsof_map_t *)free_list.next;
	head->fd = file;
	memcpy((void*)head->path, (const void*)path, strlen(path));
	list_move((struct list_head*)head, &fd_cache);
}

static void uncache_fd(FILE *file) {
	list_move((struct list_head*)fd_to_head(file), &free_list);
}

static lsof_map_t *find_fd(FILE *file) {
	lsof_map_t *fd;
	struct list_head *p;
	list_for_each(p, &fd_cache) {
		if(((lsof_map_t *)p)->fd == file) {
			return (lsof_map_t *)p;
		}
	}
	return NULL;
}

FILE *fopen(const char *path, const char *mode) {
	node_t *nod = vfs_find_node(path, NULL);
	FILE *file;
	if (nod == NULL) {
		//TODO: create file.
	}
	if (NULL == nod->fs_type->file_op->fopen) {
		LOG_ERROR("fop->fopen is NULL handler\n");
		return NULL;
	}
	file = nod->fs_type->file_op->fopen(path, mode);
	cache_fd(path, file);
	return file;
}

FILE *fdopen(int fd, const char *mode) {
	return NULL;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	node_t *nod = vfs_find_node(find_fd(file)->path, NULL);
	if (NULL == nod) {
		LOG_ERROR("node is NULL\n");
		return -1;
	}
	if (NULL == nod->fs_type->file_op->fwrite) {
		LOG_ERROR("fop->fwrite is NULL handler\n");
		return -1;
	}
	return nod->fs_type->file_op->fwrite(buf, size, count, file);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	node_t *nod = vfs_find_node(find_fd(file)->path, NULL);
	if (NULL == nod){
		LOG_ERROR("node is NULL\n");
		return -1;
	}
	if (NULL == nod->fs_type->file_op->fread) {
		LOG_ERROR("fop->fread is NULL handler\n");
		return -1;
	}
	return nod->fs_type->file_op->fread(buf, size, count, file);
}

int fclose(FILE *fp) {
	node_t *nod = vfs_find_node(find_fd(fp)->path, NULL);

	if (NULL == nod) {
		return EOF;
	}
	if (NULL == nod->fs_type->file_op->fclose) {
		return EOF;
	}
	uncache_fd(fp);
	return nod->fs_type->file_op->fclose(fp);
}
#if 0
int remove(const char *pathname) {
	ramfs_create_param_t param;
	fsop_desc_t *fsop;
	if (NULL == (fsop = rootfs_get_fsopdesc("/ramfs/"))) {
		LOG_ERROR("Can't find ramfs disk\n");
		return -1;
	}
	sprintf(param.name, basename(pathname));
	if (-1 == fsop->delete_file(param.name)) {
		LOG_ERROR("Can't delete ramfs disk\n");
		return -1;
	}
	return 0;
}
#endif

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
