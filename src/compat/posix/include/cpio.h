/**
 * @file
 * @brief Extended cpio format from POSIX.1.
 *
 * @date 27.09.10
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#ifndef CPIO_H_
#define CPIO_H_

#include <sys/stat.h>

/* A cpio archive consists of a sequence of files.
   Each file has a 76 byte header,
   a variable length, NUL terminated filename,
   and variable length file data.
   A header for a filename "TRAILER!!!" indicates the end of the archive.  */

struct cpio_entry {
	mode_t  mode;      /**< Regular permissions and the file type.*/
	uid_t   uid;       /**< User id of the owner. */
	gid_t   gid;       /**< Group id of the owner. */
	long    mtime;     /**< Modification time, since the Unix epoch start. */
	char   *name;      /**< Pathname (not necessarily NULL-terminated).*/
	size_t  name_len;  /**< Bytes in the pathname.*/
	char   *data;
	size_t  size;
};

extern char *cpio_parse_entry(const char *cpio, struct cpio_entry *entry);

#endif /* CPIO_H_ */
