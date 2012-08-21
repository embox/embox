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
	dest[11] = 0;

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
		*(dest + i) = *src;
		src++;
	}

	return dest;
}

/*
 *	Convert a filename element from directory entry (11) to canonical (8.3)
 */
uint8_t *fat_dir_to_canonical(uint8_t *dest,
		uint8_t *src, uint8_t dir) {
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

void get_filename(uint8_t *tmppath, uint8_t *filename) {
	uint8_t *p;

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
	fat_canonical_to_dir(filename, p);
	if (p > tmppath) {
		p--;
	}
	if (*p == DIR_SEPARATOR || p == tmppath) {
		*p = 0;
	}
}

void cut_mount_dir(uint8_t *path, uint8_t *mount_dir) {
		uint8_t *p;

		p = path;
		while (*mount_dir && (*mount_dir == *p)) {
			mount_dir++;
			p++;
		}
		strcpy((char *) path, (const char *) p);
	}

int fatfs_set_path (uint8_t *path, node_t *nod) {

	node_t *parent, *node;
	char buff[MAX_LENGTH_PATH_NAME];

	*path = *buff= 0;
	node = nod;
	strcpy((char *) buff, (const char *) &node->name);

	while(NULL !=
			(parent = vfs_find_parent((const char *) &node->name, node))) {
		strcpy((char *) path, (const char *) &parent->name);
		if('/' != *path) {
			strcat((char *) path, (const char *) "/");
		}
		strcat((char *) path, (const char *) buff);
		node = parent;
		strcpy((char *) buff, (const char *) path);
	}

	strncpy((char *) buff, (char *) path, MAX_LENGTH_PATH_NAME);
	buff[MAX_LENGTH_PATH_NAME - 1] = 0;
	if (strcmp((char *) path,(char *) buff)) {
		return DFS_PATHLEN;
	}
	return DFS_OK;
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
