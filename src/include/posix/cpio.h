/**
 * @file
 * @brief Extended cpio format from POSIX.1.
 *
 * @date 27.09.10
 * @author Nikolay Korotky
 */

#ifndef CPIO_H_
#define CPIO_H_

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

#define C_IRUSR	        0x100
#define C_IWUSR	        0x80
#define C_IXUSR	        0x40
#define C_IRGRP	        0x20
#define C_IWGRP	        0x10
#define C_IXGRP	        0x8
#define C_IROTH	        0x4
#define C_IWOTH	        0x2
#define C_IXOTH	        0x1

#define C_ISUID	        0x800
#define C_ISGID	        0x400
#define C_ISVTX	        0x200

#define C_ISBLK	        0x6000
#define C_ISCHR	        0x2000
#define C_ISDIR	        0x4000
#define C_ISFIFO        0x1000
#define C_ISSOCK        0xC000
#define C_ISLNK	        0xA000
#define C_ISCTG	        0x9000
#define C_ISREG	        0x8000

#endif /* CPIO_H_ */
