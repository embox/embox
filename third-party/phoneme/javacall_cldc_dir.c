/**
 * @file
 * @brief
 *
 * @date 8.01.13
 * @author Alexander Kalmuk
 */

#include <javacall_dir.h>
#include <mem/objalloc.h>
#include <string.h>
#include <fs/vfs.h>
#include "embox_java_compat.h"

struct java_dir {
	node_t *dir;
	node_t *cur_file;
};

OBJALLOC_DEF(__dir_pool, struct java_dir, 64);

javacall_handle javacall_dir_open(const javacall_utf16* path, int pathLen) {
	node_t *node;
	struct java_dir *jdir;
	javacall_int32 utf8NameLen;
	unsigned char *utf8Name;

	if (0 > utf16_to_utf8(path, pathLen, &utf8Name, &utf8NameLen)) {
		return NULL;
	}

    node = vfs_lookup(NULL, (const char *) utf8Name);
    jdir = objalloc(&__dir_pool);
    if (NULL == jdir) {
    	return NULL;
    }
    jdir->dir = node;
    jdir->cur_file = NULL;

    free(utf8Name);

    return node;
}

/* FIXME O(n^2). It may be implement in O(n) */
javacall_utf16* javacall_dir_get_next(javacall_handle handle, int* /*OUT*/ outFilenameLength) {
	node_t *item;
	javacall_utf16* name;
	javacall_int32 namelen;
	int get_this_item = 0;
	struct java_dir *jdir = (struct java_dir *)handle;

	/* FIXME look for basename. See javacall_dir.h */
	tree_foreach_children(item, (&jdir->dir->tree_link), tree_link) {
		if (NULL == jdir->cur_file) {
			break;
		}
		if (get_this_item) {
			break;
		}
		if (jdir->cur_file == item) {
			/* and get next file */
			get_this_item = 1;
		}
	}

	jdir->cur_file = item;

	/* FIXME freeing of memory will be doing in dir_close */
	utf8_to_utf16(&name, &namelen, (const unsigned char *)item->name, strlen(item->name));
	*outFilenameLength = namelen;

	return name;
}

void javacall_dir_close(javacall_handle handle) {
	/* do freeing of memory allocated in javacall_dir_get_next */
	objfree(&__dir_pool, handle);
}

javacall_int64 javacall_dir_get_free_space_for_java(void){
	/* TODO */
    return 1000000;
}

