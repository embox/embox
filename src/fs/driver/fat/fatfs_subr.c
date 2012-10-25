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
#include <fs/fat.h>
#include <fs/node.h>
#include <fs/vfs.h>

/*
 *	Convert a filename element from canonical (8.3) to directory entry (11)
 *	form src must point to the first non-separator character.
 *	dest must point to a 12-byte buffer.
 */
uint8_t *fat_canonical_to_dir(uint8_t *dest, uint8_t *src) {

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
		if (*src >= 'a' && *dest <='z') {
			*src = (*src - 'a') + 'A';
		}

		*(dest + i) = *src;
		src++;
	}

	return dest;
}

/*
 *	Convert a filename element from directory entry (11) to canonical (8.3)
 */
uint8_t *fat_dir_to_canonical(uint8_t *dest, uint8_t *src, uint8_t dir) {
        int i;
        uint8_t *dst;

        dst = dest;
        memset(dest, 0, MSDOS_NAME + 2);
        for (i = 0; i < 8; i++) {
			if (*src != ' ') {
				*dest = *src;
				if ((0 == dir) && (*dest >= 'A' && *dest <='Z')) {
					*dest = (*dest - 'A') + 'a';
				}
				dest++;
			}
			src++;
        }
        if ((*src != ' ') && (0 == dir)) {
        	*dest++ = '.';
        }
        for (i = 0; i < 3; i++) {
			if (*src != ' ') {
				*dest = *src;
				if ((0 == dir) && (*dest >= 'A' && *dest <='Z')) {
					*dest = (*dest - 'A') + 'a';
				}
				dest++;
			}
			src++;
        }
        return dst;
}

void get_filename(char *tmppath, char *filename) {
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
	fat_canonical_to_dir((uint8_t *) filename, (uint8_t *) p);
	if (p > tmppath) {
		p--;
	}
	if (*p == DIR_SEPARATOR || p == tmppath) {
		*p = 0;
	}
}

void set_filetime(dir_ent_t *de) {
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
