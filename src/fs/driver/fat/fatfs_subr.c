/**
 * @file
 * @brief
 *
 * @date 14.08.2012
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <fs/hlpr_path.h>
#include <fs/fat.h>

int fat_check_filename(char *filename) {
	char *point;
	int len;
	int extlen;
	/* filename.ext <= 8 + 3 + dot */
	if(MSDOS_NAME + 1 < (len = strlen(filename))) {
		return ENAMETOOLONG;
	}
	point = filename + len;
	extlen = 0;

	/* set point to a dot */
	do {
		if(*point == '.') {
			break;
		}
		point --;
		extlen++;

	} while (point > filename);

	if(*point == '.') {
		if(extlen > 4) {
			/* normal only if name is .filename */
			if(point != filename) {
				return EINVAL;
			}
		} else {
			if(len - extlen > 8) {
				return ENAMETOOLONG;
			}
			if(1 >= extlen) {
				return EINVAL;
			}
		}
	} else if(len > 8) {
		return ENAMETOOLONG;
	}
	return 0;
}
#if 0
void fat_get_filename(char *tmppath, char *filename) {
	char *p;

	p = tmppath;
	/* strip leading path separators */
	while (*tmppath == DIR_SEPARATOR) {
		strcpy((char *) tmppath, (char *) tmppath + 1);
	}
	while (*(p++));
	p--;
	while (p > tmppath && *p != DIR_SEPARATOR) {
		p--;
	}

	if (*p == DIR_SEPARATOR) {
		p++;
	}

	return;
}
#endif
void fat_set_filetime(struct fat_dirent *de) {
	/* TODO set normal time */
		de->crttime_l = 0x20;	/* 01:01:00am, Jan 1, 2006. */
		de->crttime_h = 0x08;
		de->crtdate_l = 0x11;
		de->crtdate_h = 0x34;
		de->lstaccdate_l = 0x11;
		de->lstaccdate_h = 0x34;
		de->wrttime_l = 0x20;
		de->wrttime_h = 0x08;
		de->wrtdate_l = 0x11;
		de->wrtdate_h = 0x34;
}

/*
 *	Convert a filename element from canonical (8.3) to directory entry (11)
 *	form src must point to the first non-separator character.
 *	dest must point to a 12-byte buffer.
 */
char *path_canonical_to_dir(char *dest, char *src) {

	memset(dest, (int)' ', MSDOS_NAME);
	dest[MSDOS_NAME] = 0;

	for (int i = 0; i <= 11; i++) {
		if (!*src) {
			break;
		}
		if (*src == '/') {
			break;
		}
		if (*src == '.') {
			i = 7;
			src++;
			continue;
		}
		if (*src >= 'a' && *src <='z') {
			*(dest + i) = (*src - 'a') + 'A';
		} else {
			*(dest + i) = *src;
		}
		src++;
	}

	return dest;
}

/*
 *	Convert a filename element from directory entry (11) to canonical (8.3)
 */
char *path_dir_to_canonical(char *dest, char *src, char dir) {
        int i;
        char *dst;

        dst = dest;
        memset(dest, 0, MSDOS_NAME + 2);
        for (i = 0; i < 8; i++) {
			if (*src != ' ') {
				*dst = *src;
				if (*dst >= 'A' && *dst <='Z') {
					*dst = (*dst - 'A') + 'a';
				}
				dst++;
			}
			src++;
        }
        if ((*src != ' ') && (0 == dir)) {
        	*dst++ = '.';
        }
        for (i = 0; i < 3; i++) {
			if (*src != ' ') {
				*dst = *src;
				if (*dst >= 'A' && *dst <='Z') {
					*dst = (*dst - 'A') + 'a';
				}
				dst++;
			}
			src++;
        }
        return dest;
}


