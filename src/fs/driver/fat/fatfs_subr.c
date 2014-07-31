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
#include <fs/fat.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/hlpr_path.h>


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

void fat_get_filename(char *tmppath, char *filename) {
	char *p;

	p = tmppath;
	/* strip leading path separators */
	while (*tmppath == DIR_SEPARATOR){
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
	path_canonical_to_dir(filename, p);
	if (p > tmppath) {
		p--;
	}
	if (*p == DIR_SEPARATOR || p == tmppath) {
		*p = 0;
	}
}

void fat_set_filetime(dir_ent_t *de) {
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
