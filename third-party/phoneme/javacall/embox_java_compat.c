/**
 * @file
 * @brief
 *
 * @date 7.01.13
 * @author Alexander Kalmuk
 */

#include <javautil_unicode.h>
#include <errno.h>
#include <stdlib.h>
#include "embox_java_compat.h"

/* TODO */
javacall_result emboxErrno2javaErrno(int embox_errno) {
	switch (errno) {
	case ENOERR:
		return JAVACALL_OK;
	case EBADF:
		return JAVACALL_BAD_FILE_NAME;
	case ENOMEM:
		return JAVACALL_OUT_OF_MEMORY;
	}

	return JAVACALL_FAIL;
}

javacall_result utf16_to_utf8(const javacall_utf16* pUtf16,
                                               javacall_int32 utf16Len,
                                               unsigned char** pUtf8,
                                               javacall_int32 *utf8Len) {
	int res;

	if (0 > (res = javautil_unicode_utf16_utf8length(pUtf16, utf8Len))) {
		return res;
	}

	if ((*pUtf8 = malloc(*utf8Len)) == NULL) {
		return JAVACALL_FAIL;
	}

	if (0 > (res = javautil_unicode_utf16_to_utf8(pUtf16, utf16Len,
			*pUtf8, *utf8Len, utf8Len))) {
		free(*pUtf8);
		return res;
	}

	return res;
}

javacall_result utf8_to_utf16(javacall_utf16** pUtf16,
                                               javacall_int32 *utf16Len,
                                               const unsigned char *pUtf8,
                                               javacall_int32 utf8Len) {
	int i;

	if ((*pUtf16 = malloc(utf8Len * 2 + 2)) == NULL) {
		return JAVACALL_FAIL;
	}

	for (i = 0; i <= utf8Len; i++) {
		*(*pUtf16 + i) = *(pUtf8 + i);
	}

	return JAVACALL_OK;
}

