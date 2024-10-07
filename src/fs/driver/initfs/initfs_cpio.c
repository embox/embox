/**
 * @file
 * @brief CPIO file format.
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 *          - Rewrite from scratch, no more dependencies on initfs.
 */

#include <cpio.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include <fs/fs_driver.h>
#include <kernel/printk.h>

/**
 * The pathname is followed by NUL bytes so that the total size of the fixed
 * header plus pathname is a multiple of four. Likewise, the file data is
 * padded to a multiple of four bytes.
 */
#define NEWC_NAME_ALIGN(len) ((((len) + 1) & ~3) + 2)
#define NEWC_FILE_ALIGN(len) (((len) + 3) & ~3)

/* Value for the field `c_magic'.  */
#define NEWC_MAGIC       "070701"
#define OLD_BIN_MAGIC    "070707"

#define NEWC_FIELD_SZ     8

/* New ASCII Format */
struct newc_hdr {
	char c_magic[sizeof(NEWC_MAGIC) - 1]; /**< Must be "070701". */
	char c_ino[NEWC_FIELD_SZ];       /**< The inode numbers from the disk. */
	char c_mode[NEWC_FIELD_SZ];      /**< Permissions and the file type.*/
	char c_uid[NEWC_FIELD_SZ];       /**< User id of the owner. */
	char c_gid[NEWC_FIELD_SZ];       /**< Group id of the owner. */
	char c_nlink[NEWC_FIELD_SZ];     /**< The number of links to this file.*/
	char c_mtime[NEWC_FIELD_SZ];     /**< Modification time, seconds. */
	char c_filesize[NEWC_FIELD_SZ];  /**< Zero for FIFOs and directories. */
	char c_devmajor[NEWC_FIELD_SZ];
	char c_devminor[NEWC_FIELD_SZ];
	char c_rdevmajor[NEWC_FIELD_SZ];
	char c_rdevminor[NEWC_FIELD_SZ];
	char c_namesize[NEWC_FIELD_SZ];  /**< Bytes in the pathname.*/
	char c_check[NEWC_FIELD_SZ];     /**< Ignored, must be zero. */
};

struct newc_hdr_parsed {
	unsigned long ino;
	unsigned long mode;
	unsigned long uid;
	unsigned long gid;
	unsigned long nlink;
	unsigned long mtime;
	unsigned long filesize;
	unsigned long devmajor;
	unsigned long devminor;
	unsigned long rdevmajor;
	unsigned long rdevminor;
	unsigned long namesize;
};

#define STR_LEN_TUPLE(str) (str), (sizeof(str) - 1)

static char *cpio_parse_newc(const struct newc_hdr *hdr,
		struct cpio_entry *entry) {
	char *raw = (char *) hdr + sizeof(hdr->c_magic);
	struct newc_hdr_parsed parsed;
	char field_buff[NEWC_FIELD_SZ + 1];

	field_buff[NEWC_FIELD_SZ] = '\0';

	for (unsigned long *p_field = (unsigned long *) &parsed;
	     p_field < (unsigned long *) (&parsed + 1);
	     ++p_field, raw += NEWC_FIELD_SZ) {
		unsigned long tmp;

		memcpy(field_buff, raw, NEWC_FIELD_SZ);
		tmp = strtol(field_buff, NULL, 16);
		memcpy(p_field, &tmp, sizeof *p_field);
	}

	raw += NEWC_FIELD_SZ; /* skip c_check field, now raw points to name. */

	if (strncmp(raw, STR_LEN_TUPLE("TRAILER!!!")) == 0) {
		return NULL;
	}

	entry->mode  = parsed.mode;
	entry->uid   = parsed.uid;
	entry->gid   = parsed.gid;
	entry->mtime = parsed.mtime;
#if 0
	entry->ino   = parsed.ino;
	entry->nlink = parsed.nlink;
	entry->devmajor = parsed.devmajor;
	entry->devminor = parsed.devminor;
	entry->rdevmajor = parsed.rdevmajor;
	entry->rdevminor = parsed.rdevminor;
#endif

	entry->name = raw;
	entry->name_len = parsed.namesize;
	raw += NEWC_NAME_ALIGN(parsed.namesize);

	entry->data = raw;
	entry->size = parsed.filesize;
	raw += NEWC_FILE_ALIGN(parsed.filesize);

	return raw;
}

char *cpio_parse_entry(const char *cpio, struct cpio_entry *entry) {
	if (memcmp(cpio, STR_LEN_TUPLE(OLD_BIN_MAGIC)) == 0) {
		printk("Use -H newc option for create CPIO archive\n");
		return NULL;
	}
	if (memcmp(cpio, STR_LEN_TUPLE(NEWC_MAGIC)) != 0) {
		printk("Newc ASCII CPIO format not recognized\n");
		return NULL;
	}

	return cpio_parse_newc((struct newc_hdr *) cpio, entry);
}

