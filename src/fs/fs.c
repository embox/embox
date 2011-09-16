/**
 * @file
 *
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fs/fs.h>


int remove(const char *pathname) {
	return 0;
}


int unlink(const char *pathname) {
	return 0;
}

int rmdir(const char *pathname) {
	return 0;
}

int creat(const char *pathname, mode_t mode) {
	return 0;

}

int fstat(const char *path, stat_t *buf) {
#if 0
	//FIXME: workaround, ramfs depend.
	node_t *nod;
	ramfs_file_description_t *desc;
	nod = vfs_find_node(path, NULL);
	desc = (ramfs_file_description_t *) nod->attr;
	buf->st_size = desc->size;
	buf->st_mode = desc->mode;
	buf->st_mtime = desc->mtime;
#endif
	return 0;
}

