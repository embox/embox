/**
 * @file
 * @brief
 *
 * @date 27.12.12
 * @author Alexander Kalmuk
 */

#include <javacall_file.h>
#include <javacall_dir.h>
#include <javautil_unicode.h>
#include <javacall_memory.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

static javacall_result emboxErrno2javaErrno(int embox_errno);

javacall_result javacall_file_open(javacall_const_utf16_string fileName,
                                  int fileNameLen,
                                  int flags,
                                  javacall_handle* /* OUT */ handle) {
	int res;
	javacall_int32 utf8NameLen;
	unsigned char *utf8Name;

	if (0 > (res = javautil_unicode_utf16_utf8length(fileName, &utf8NameLen))) {
		return res;
	}

	if ((utf8Name = malloc(utf8NameLen)) == NULL) {
		return JAVACALL_FAIL;
	}

	if (0 > (res = javautil_unicode_utf16_to_utf8(fileName, fileNameLen,
			utf8Name, utf8NameLen, &utf8NameLen))) {
		return res;
	}

	handle = (void*)open((const char *)utf8Name, flags);
	free(utf8Name);

	return emboxErrno2javaErrno(errno);
}

/* TODO */
static javacall_result emboxErrno2javaErrno(int embox_errno) {
	switch (errno) {
	case ENOERR:
		return JAVACALL_OK;
	case EBADF:
		return JAVACALL_BAD_FILE_NAME;
	case ENOMEM:
		return JAVACALL_OUT_OF_MEMORY;
	}

	return JAVACALL_OK;
}
