/**
 * @file
 * @brief
 *
 * @date 8.01.13
 * @author Alexander Kalmuk
 */

#include <javacall_dir.h>
#include <string.h>
#include <dirent.h>
#include "embox_java_compat.h"

javacall_handle javacall_dir_open(const javacall_utf16* path, int pathLen) {
	DIR *dir;
	javacall_int32 utf8NameLen;
	unsigned char *utf8Name;

	if (0 > utf16_to_utf8(path, pathLen, &utf8Name, &utf8NameLen)) {
		return NULL;
	}

	dir = opendir((const char *) utf8Name);

    free(utf8Name);

	return dir;
}

/* FIXME O(n^2). It may be implement in O(n) */
javacall_utf16* javacall_dir_get_next(javacall_handle handle, int* /*OUT*/ outFilenameLength) {
	javacall_utf16* name;
	javacall_int32 namelen;
	struct dirent *dent;

	dent = readdir((DIR *)handle);
	if (dent == NULL) {
		return NULL;
	}

	/* FIXME freeing of memory will be doing in dir_close */
	utf8_to_utf16(&name, &namelen, (const unsigned char *)dent->d_name, strlen(dent->d_name));
	*outFilenameLength = namelen;

	return name;
}

void javacall_dir_close(javacall_handle handle) {
	closedir((DIR *)handle);
}

javacall_int64 javacall_dir_get_free_space_for_java(void){
	/* TODO */
    return 1000000;
}

