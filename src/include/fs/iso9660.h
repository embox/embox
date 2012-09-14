/**
 * @file
 * @brief
 *
 * @date 20.08.2012
 * @author Andrey Gazukin
 */


#ifndef ISO9660_H_
#define ISO9660_H_


/*
 * iso9660.h
 *
 * ISO-9660 CD-ROM file system definitions
 *
 * Copyright (C) 2002 Michael Ringgaard. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/stat.h>
#include <types.h>
#include <fs/vfs.h>

#define ISODCL(from, to) (to - from + 1)


#define ISO_VD_BOOT         0
#define ISO_VD_PRIMARY      1
#define ISO_VD_SUPPLEMENTAL 2
#define ISO_VD_PARTITION    3
#define ISO_VD_END          255

#pragma pack(push, 1)

typedef struct iso_volume_descriptor {
  unsigned char type                    [ISODCL(  1,   1)]; /* 711 */
  unsigned char id                      [ISODCL(  2,   6)];
  unsigned char version                 [ISODCL(  7,   7)]; /* 711 */
  unsigned char flags                   [ISODCL(  8,   8)];
  unsigned char system_id               [ISODCL(  9,  40)]; /* achars */
  unsigned char volume_id               [ISODCL( 41,  72)]; /* dchars */
  unsigned char unused1                 [ISODCL( 73,  80)];
  unsigned char volume_space_size       [ISODCL( 81,  88)]; /* 733 */
  unsigned char escape_sequences        [ISODCL( 89, 120)];
  unsigned char volume_set_size         [ISODCL(121, 124)]; /* 723 */
  unsigned char volume_sequence_number  [ISODCL(125, 128)]; /* 723 */
  unsigned char logical_block_size      [ISODCL(129, 132)]; /* 723 */
  unsigned char path_table_size         [ISODCL(133, 140)]; /* 733 */
  unsigned char type_l_path_table       [ISODCL(141, 144)]; /* 731 */
  unsigned char opt_type_l_path_table   [ISODCL(145, 148)]; /* 731 */
  unsigned char type_m_path_table       [ISODCL(149, 152)]; /* 732 */
  unsigned char opt_type_m_path_table   [ISODCL(153, 156)]; /* 732 */
  unsigned char root_directory_record   [ISODCL(157, 190)]; /* 9.1 */
  unsigned char volume_set_id           [ISODCL(191, 318)]; /* dchars */
  unsigned char publisher_id            [ISODCL(319, 446)]; /* achars */
  unsigned char preparer_id             [ISODCL(447, 574)]; /* achars */
  unsigned char application_id          [ISODCL(575, 702)]; /* achars */
  unsigned char copyright_file_id       [ISODCL(703, 739)]; /* 7.5 dchars */
  unsigned char abstract_file_id        [ISODCL(740, 776)]; /* 7.5 dchars */
  unsigned char bibliographic_file_id   [ISODCL(777, 813)]; /* 7.5 dchars */
  unsigned char creation_date           [ISODCL(814, 830)]; /* 8.4.26.1 */
  unsigned char modification_date       [ISODCL(831, 847)]; /* 8.4.26.1 */
  unsigned char expiration_date         [ISODCL(848, 864)]; /* 8.4.26.1 */
  unsigned char effective_date          [ISODCL(865, 881)]; /* 8.4.26.1 */
  unsigned char file_structure_version  [ISODCL(882, 882)]; /* 711 */
  unsigned char unused2                 [ISODCL(883, 883)];
  unsigned char application_data        [ISODCL(884, 1395)];
  unsigned char unused3                 [ISODCL(1396, 2048)];
} iso_volume_descriptor_t;

typedef struct iso_directory_record {
  unsigned char length                  [ISODCL( 1,  1)]; /* 711 */
  unsigned char ext_attr_length         [ISODCL( 2,  2)]; /* 711 */
  unsigned char extent                  [ISODCL( 3, 10)]; /* 733 */
  unsigned char size                    [ISODCL(11, 18)]; /* 733 */
  unsigned char date                    [ISODCL(19, 25)]; /* 7 by 711 */
  unsigned char flags                   [ISODCL(26, 26)];
  unsigned char file_unit_size          [ISODCL(27, 27)]; /* 711 */
  unsigned char interleave              [ISODCL(28, 28)]; /* 711 */
  unsigned char volume_sequence_number  [ISODCL(29, 32)]; /* 723 */
  unsigned char name_len                [ISODCL(33, 33)]; /* 711 */
  unsigned char name                    [0];
} iso_directory_record_t;

typedef struct iso_extended_attributes {
  unsigned char owner                   [ISODCL(  1,   4)]; /* 723 */
  unsigned char group                   [ISODCL(  5,   8)]; /* 723 */
  unsigned char perm                    [ISODCL(  9,  10)]; /* 9.5.3 */
  unsigned char ctime                   [ISODCL( 11,  27)]; /* 8.4.26.1 */
  unsigned char mtime                   [ISODCL( 28,  44)]; /* 8.4.26.1 */
  unsigned char xtime                   [ISODCL( 45,  61)]; /* 8.4.26.1 */
  unsigned char ftime                   [ISODCL( 62,  78)]; /* 8.4.26.1 */
  unsigned char recfmt                  [ISODCL( 79,  79)]; /* 711 */
  unsigned char recattr                 [ISODCL( 80,  80)]; /* 711 */
  unsigned char reclen                  [ISODCL( 81,  84)]; /* 723 */
  unsigned char system_id               [ISODCL( 85, 116)]; /* achars */
  unsigned char system_use              [ISODCL(117, 180)];
  unsigned char version                 [ISODCL(181, 181)]; /* 711 */
  unsigned char len_esc                 [ISODCL(182, 182)]; /* 711 */
  unsigned char reserved                [ISODCL(183, 246)];
  unsigned char len_au                  [ISODCL(247, 250)]; /* 723 */
} iso_extended_attributes_t;

typedef struct iso_pathtable_record {
  unsigned char length;
  unsigned char ext_attr_length;
  unsigned long extent;
  unsigned short int parent;
  char name[0];
} iso_pathtable_record_t;

#pragma pack(pop)
/*
 * This option is necessary to deal with align by type
 * sizeof(iso_pathtable_record_t) returned 12
 */
#define PATHTABLE_SIZE  8

#define CDFS_DEFAULT_CACHESIZE 128
#define CDFS_BLOCKSIZE         2048

#define PS1                     '/'     /* Primary path separator */
#define PS2                     '\\'    /* Alternate path separator */


#define S_IFMT         0170000   /* File type mask */
#define S_IFPKT        0160000   /* Packet device */
#define S_IFSOCK       0140000   /* Socket */
#define S_IFLNK        0120000   /* Symbolic link */
#define S_IFREG        0100000   /* Regular file */
#define S_IFBLK        0060000   /* Block device */
#define S_IFDIR        0040000   /* Directory */
#define S_IFCHR        0020000   /* Character device */
#define S_IFIFO        0010000   /* Pipe */

#define S_IREAD        0000400   /* Read permission, owner */
#define S_IWRITE       0000200   /* Write permission, owner */
#define S_IEXEC        0000100   /* Execute/search permission, owner */

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)
#define S_ISPKT(m)      (((m) & S_IFMT) == S_IFPKT)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#define F_MODIFIED     0x100000  /* File has been modified since it was opened */
#define F_DIR          0x200000  /* File is a directory */
#define F_CLOSED       0x400000  /* File is closed */

typedef unsigned short wchar_t;

#define __int64 long long
typedef __int64 off64_t;

typedef struct cdfs {
	dev_t devno;
	int blks;
	int volblks;
	int vdblk;
	int joliet;
	unsigned char *path_table_buffer;
	struct iso_pathtable_record **path_table;
	int path_table_records;
} cdfs_t;

typedef struct cdfs_file {
	int extent;
	int size;
	time_t date;
} cdfs_file_t;

#define VFS_LOCK_TIMEOUT        60000     /* Timeout for file system locks */

#define F_MODIFIED              0x100000  /* File has been modified since it was opened */
#define F_DIR                   0x200000  /* File is a directory */
#define F_CLOSED                0x400000  /* File is closed */

#define FSOP_MKFS       0x00000001
#define FSOP_MOUNT      0x00000002
#define FSOP_UMOUNT     0x00000004
#define FSOP_STATFS     0x00000008
#define FSOP_OPEN       0x00000010
#define FSOP_CLOSE      0x00000020
#define FSOP_FSYNC      0x00000040
#define FSOP_READ       0x00000080
#define FSOP_WRITE      0x00000100
#define FSOP_IOCTL      0x00000200
#define FSOP_TELL       0x00000400
#define FSOP_LSEEK      0x00000800
#define FSOP_FTRUNCATE  0x00001000
#define FSOP_FUTIME     0x00002000
#define FSOP_UTIME      0x00004000
#define FSOP_FSTAT      0x00008000
#define FSOP_STAT       0x00010000
#define FSOP_ACCESS     0x00020000
#define FSOP_FCHMOD     0x00040000
#define FSOP_CHMOD      0x00080000
#define FSOP_FCHOWN     0x00100000
#define FSOP_CHOWN      0x00200000
#define FSOP_MKDIR      0x00400000
#define FSOP_RMDIR      0x00800000
#define FSOP_RENAME     0x01000000
#define FSOP_LINK       0x02000000
#define FSOP_UNLINK     0x04000000
#define FSOP_OPENDIR    0x08000000
#define FSOP_READDIR    0x10000000


typedef struct cdfs_fs_description {
	char *name;
	dev_t devnum;
	//node_t *dev_node;
	char mntfrom[MAX_LENGTH_PATH_NAME];
	char mntto[MAX_LENGTH_PATH_NAME];
	struct fsops *ops;
	mode_t mode;
	uid_t uid;
	gid_t gid;
	void *data;
} cdfs_fs_description_t;

typedef struct cdfs_file_description {
	int flags;
	int mode;
	uid_t owner;
	gid_t group;
	off64_t pos;
	void *data;
	char *path;
	char chbuf;
	cdfs_fs_description_t *fs;
} cdfs_file_description_t;


typedef struct buf {
	blkno_t blkno;
	char *data;
} buf_t;

typedef struct direntry {
	ino_t ino;
	unsigned int reclen;
	unsigned int namelen;
	char name[MAX_LENGTH_PATH_NAME];
} direntry_t;

typedef struct statfs  {
	unsigned int bsize;        /* Fundamental file system block size */
	unsigned int iosize;       /* Optimal transfer block size */
	unsigned int blocks;       /* Total data blocks in file system */
	unsigned int bfree;        /* Free blocks in fs */
	unsigned int files;        /* Total file nodes in file system */
	unsigned int ffree;        /* Free file nodes in fs */
	unsigned int cachesize;    /* Cache buffers */
	char fstype[MAX_LENGTH_FILE_NAME];   /* File system type name */
	char mntto[MAX_LENGTH_PATH_NAME];       /* Directory on which mounted */
	char mntfrom[MAX_LENGTH_PATH_NAME];     /* Mounted file system */
} statfs_t;

struct fsops {
	unsigned long reentrant;

	int (*lockfs)(cdfs_fs_description_t *fs);
	void (*unlockfs)(cdfs_fs_description_t *fs);

	int (*mkfs)(char *devname, char *opts);
	int (*mount)(node_t *root_node);
	int (*umount)(cdfs_fs_description_t *fs);

	int (*statfs)(cdfs_fs_description_t *fs, statfs_t *buf);

	int (*open)(cdfs_file_description_t *filp, char *name);
	int (*close)(cdfs_file_description_t *filp);
	int (*destroy)(cdfs_file_description_t *filp);
	int (*fsync)(cdfs_file_description_t *filp);

	int (*read)(cdfs_file_description_t *filp, void *data, size_t size, off64_t pos);
	int (*write)(cdfs_file_description_t *filp, void *data, size_t size, off64_t pos);
	int (*ioctl)(cdfs_file_description_t *filp, int cmd, void *data, size_t size);

	off64_t (*tell)(cdfs_file_description_t *filp);
	off64_t (*lseek)(cdfs_file_description_t *filp, off64_t offset, int origin);
	int (*ftruncate)(cdfs_file_description_t *filp, off64_t size);

	/*int (*futime)(cdfs_file_description_t *filp, struct utimbuf *times); */
	/*int (*utime)(cdfs_fs_description_t *fs, char *name, struct utimbuf *times); */

	int (*fstat)(cdfs_file_description_t *filp, stat_t *buffer);
	int (*stat)(cdfs_fs_description_t *fs, char *name, stat_t *buffer);

	int (*access)(cdfs_fs_description_t *fs, char *name, int mode);

	int (*fchmod)(cdfs_file_description_t *filp, int mode);
	int (*chmod)(cdfs_fs_description_t *fs, char *name, int mode);
	int (*fchown)(cdfs_file_description_t *filp, int owner, int group);
	int (*chown)(cdfs_fs_description_t *fs, char *name, int owner, int group);

	int (*mkdir)(cdfs_fs_description_t *fs, char *name, int mode);
	int (*rmdir)(cdfs_fs_description_t *fs, char *name);

	int (*rename)(cdfs_fs_description_t *fs, char *oldname, char *newname);
	int (*link)(cdfs_fs_description_t *fs, char *oldname, char *newname);
	int (*unlink)(cdfs_fs_description_t *fs, char *name);

	int (*opendir)(cdfs_file_description_t *filp, char *name);
	int (*readdir)(cdfs_file_description_t *filp, direntry_t *dirp, int count);
};
/*
struct tm {
	int tm_sec;                   // Seconds after the minute [0, 59]
	int tm_min;                   // Minutes after the hour [0, 59]
	int tm_hour;                  // Hours since midnight [0, 23]
	int tm_mday;                  // Day of the month [1, 31]
	int tm_mon;                   // Months since January [0, 11]
	int tm_year;                  // Years since 1900
	int tm_wday;                  // Days since Sunday [0, 6]
	int tm_yday;                  // Days since January 1 [0, 365]
	int tm_isdst;                 // Daylight Saving Time flag
	int tm_gmtoff;                // Seconds east of UTC
	char *tm_zone;                // Timezone abbreviation
};
*/

#endif /* ISO9660_H_ */
