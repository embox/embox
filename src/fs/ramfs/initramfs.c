/**
 * @file
 * @brief parse cpio initramfs
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 */
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <lib/list.h>
#include <lib/cpio.h>
#include <stdlib.h>
#include <string.h>

/**
 * The pathname is followed by NUL bytes so that the total size of the fixed
 * header plus pathname is a multiple of four. Likewise, the file data is
 * padded to a multiple of four bytes.
 */
#define N_ALIGN(len) ((((len) + 1) & ~3) + 2)
#define F_ALIGN(len) (((len) + 3) & ~3)

static file_system_driver_t *init_fs;

static cpio_newc_header_t *parse_item(cpio_newc_header_t *cpio_h, char *name) {
	char *s;
	size_t i;
	unsigned long parsed[12], file_size, start_addr, mode, mtime;
	ramfs_create_param_t param;
	char buf[9];
	buf[8] = '\0';
	if (memcmp(cpio_h->c_magic, MAGIC_OLD_BINARY, 6)==0) {
		TRACE("Use -H newc option for create cpio arch\n");
		return NULL;
	}
	if (memcmp(cpio_h->c_magic, MAGIC_NEWC, 6)) {
		TRACE("Newc ASCII CPIO format not recognized.\n");
		return NULL;
	}
	s = (char*) cpio_h;
	for (i = 0, s += 6; i < 12; i++, s += 8) {
		memcpy(buf, s, 8);
		parsed[i] = strtol(buf, NULL, 16);
	}

	strncpy(name, (char*) cpio_h + sizeof(cpio_newc_header_t), parsed[11]);
	name[parsed[11]] = '\0';
	file_size  = parsed[6];
	start_addr = (unsigned long)cpio_h + sizeof(cpio_newc_header_t) + N_ALIGN(parsed[11]);
	mode       = parsed[1];
	mtime      = parsed[5];

	if (0 == strcmp(name, "TRAILER!!!")) {
		return NULL;
	} else {
		//TODO: set start_addr, file_size, mode.
		strncpy(param.name, name, parsed[11]);
		param.size = file_size;
		param.mode = mode;
		param.mtime = mtime;
		param.start_addr = start_addr;
		init_fs->fsop->create_file(&param);
	}
	return (cpio_newc_header_t*) F_ALIGN(start_addr + file_size);
}

int unpack_to_rootfs(void) {
	extern char _ramfs_start, _ramfs_end;
	cpio_newc_header_t *cpio_h, *cpio_next;
	char buff_name[CONFIG_MAX_LENGTH_FILE_NAME];

	if (&_ramfs_end == &_ramfs_start) {
		TRACE("No availible initramfs\n");
		return -1;
	}
	TRACE("cpio initramfs at 0x%08x\n", (unsigned int)&_ramfs_start);

	init_fs = find_filesystem("ramfs");

	cpio_h = (cpio_newc_header_t *)&_ramfs_start;
	while (NULL != (cpio_next = parse_item(cpio_h, buff_name))) {
		cpio_h = cpio_next;
	}

	return 0;
}
