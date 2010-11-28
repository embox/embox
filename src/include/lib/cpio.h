/**
 * @file
 * @brief Extended cpio format from POSIX.1.
 *
 * @date 27.09.10
 * @author Nikolay Korotky
 */
#ifndef _CPIO_H
#define _CPIO_H

/* A cpio archive consists of a sequence of files.
   Each file has a 76 byte header,
   a variable length, NUL terminated filename,
   and variable length file data.
   A header for a filename "TRAILER!!!" indicates the end of the archive.  */

/* New ASCII Format */
typedef struct cpio_newc_header {
	/* must be "070701" */
	char c_magic[6];
	/** The inode numbers from the disk. */
	char c_ino[8];
	/** The mode specifies both the regular permissions and the file type.*/
	char c_mode[8];
	/** The numeric user id and group id of the owner. */
	char c_uid[8];
	char c_gid[8];
	/** The number of links to this file.*/
	char c_nlink[8];
	/** Modification time of the file, indicated as the
	    number of seconds since the start of the Unix epoch. */
	char c_mtime[8];
	/** Must be 0 for FIFOs and directories. */
	char c_filesize[8];
	char c_devmajor[8];
	char c_devminor[8];
	char c_rdevmajor[8];
	char c_rdevminor[8];
	/** The number of bytes in the pathname that follows the header.*/
	char c_namesize[8];
	/** This field is always set to zero by writers and ignored by
	    readers.  See the next section for more details. */
	char c_check[8];
} cpio_newc_header_t;

/* Value for the field `c_magic'.  */
#define MAGIC_NEWC       "070701"
#define MAGIC_OLD_BINARY "070707"

/* Values for c_mode, OR'd together: */

#define C_IRUSR	        000400
#define C_IWUSR	        000200
#define C_IXUSR	        000100
#define C_IRGRP	        000040
#define C_IWGRP	        000020
#define C_IXGRP	        000010
#define C_IROTH	        000004
#define C_IWOTH	        000002
#define C_IXOTH	        000001

#define C_ISUID	        004000
#define C_ISGID	        002000
#define C_ISVTX	        001000

#define C_ISBLK	        060000
#define C_ISCHR	        020000
#define C_ISDIR	        040000
#define C_ISFIFO        010000
#define C_ISSOCK        0140000
#define C_ISLNK	        0120000
#define C_ISCTG	        0110000
#define C_ISREG	        0100000

#endif /* _CPIO_H */
