/*
 * Copyright (c) 2005-2007, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _FATFS_H
#define _FATFS_H

#include <sys/cdefs.h>
#include <sys/types.h>

#include "dirent.h"

/* #define DEBUG_FATFS 1 */

#ifdef DEBUG_FATFS
#define DPRINTF(a)	dprintf a
#define ASSERT(e)	dassert(e)
#else
#define DPRINTF(a)	do {} while (0)
#define ASSERT(e)
#endif


#if CONFIG_FS_THREADS > 1
#define malloc(s)		malloc_r(s)
#define free(p)			free_r(p)
#else
#define mutex_init(m)		do {} while (0)
#define mutex_destroy(m)	do {} while (0)
#define mutex_lock(m)		do {} while (0)
#define mutex_unlock(m)		do {} while (0)
#define mutex_trylock(m)	do {} while (0)
#endif


#define SEC_SIZE	512		/* sector size */
#define SEC_INVAL	0xffffffff	/* invalid sector */

/*
 * Pre-defined cluster number
 */
#define CL_ROOT		0		/* cluster 0 means the root directory */
#define CL_FREE		0		/* cluster 0 also means the free cluster */
#define CL_FIRST	2		/* first legal cluster */
#define CL_LAST		0xfffffff5	/* last legal cluster */
#define CL_EOF		0xffffffff	/* EOF cluster */

#define EOF_MASK	0xfffffff8	/* mask of eof */

#define FAT12_MASK	0x00000fff
#define FAT16_MASK	0x0000ffff

#if defined(__SUNPRO_C)
#pragma pack(1)
#endif

/*
 * BIOS parameter block
 */
struct fat_bpb {
	uint16_t	jmp_instruction;
	uint8_t		nop_instruction;
	uint8_t		oem_id[8];
	uint16_t	bytes_per_sector;
	uint8_t		sectors_per_cluster;
	uint16_t	reserved_sectors;
	uint8_t		num_of_fats;
	uint16_t	root_entries;
	uint16_t	total_sectors;
	uint8_t		media_descriptor;
	uint16_t	sectors_per_fat;
	uint16_t	sectors_per_track;
	uint16_t	heads;
	uint32_t	hidden_sectors;
	uint32_t	big_total_sectors;
	uint8_t		physical_drive;
	uint8_t		reserved;
	uint8_t		ext_boot_signature;
	uint32_t	serial_no;
	uint8_t		volume_id[11];
	uint8_t		file_sys_id[8];
} __packed;

/*
 * FAT directory entry
 */
struct fat_dirent {
	uint8_t		name[11];
	uint8_t		attr;
	uint8_t		reserve[10];
	uint16_t	time;
	uint16_t	date;
	uint16_t	cluster;
	uint32_t	size;
};

#if defined(__SUNPRO_C)
#pragma pack()
#endif

#define SLOT_EMPTY	0x00
#define SLOT_DELETED	0xe5

#define DIR_PER_SEC     (SEC_SIZE / sizeof(struct fat_dirent))

/*
 * FAT attribute for attr
 */
#define FA_RDONLY	0x01
#define FA_HIDDEN	0x02
#define FA_SYSTEM	0x04
#define FA_VOLID	0x08
#define FA_SUBDIR	0x10
#define FA_ARCH		0x20
#define FA_DEVICE	0x40

#define IS_DIR(de)	(((de)->attr) & FA_SUBDIR)
#define IS_VOL(de)	(((de)->attr) & FA_VOLID)
#define IS_FILE(de)	(!IS_DIR(de) && !IS_VOL(de))

#define IS_DELETED(de)  ((de)->name[0] == 0xe5)
#define IS_EMPTY(de)    ((de)->name[0] == 0)

/*
 * Mount data
 */
struct fatfsmount {
	int	fat_type;	/* 12 or 16 */
	u_long	root_start;	/* start sector for root directory */
	u_long	fat_start;	/* start sector for fat entries */
	u_long	data_start;	/* start sector for data */
	u_long	fat_eof;	/* id of end cluster */
	u_long	sec_per_cl;	/* sectors per cluster */
	u_long	cluster_size;	/* cluster size */
	u_long	last_cluster;	/* last cluser */
	u_long	fat_mask;	/* mask for cluster# */
	u_long	free_scan;	/* start cluster# to free search */
	vnode_t	root_vnode;	/* vnode for root */
	char	*io_buf;	/* local data buffer */
	char	*fat_buf;	/* buffer for fat entry */
	char	*dir_buf;	/* buffer for directory entry */
	dev_t	dev;		/* mounted device */
#if CONFIG_FS_THREADS > 1
	mutex_t lock;		/* file system lock */
#endif
};

#define FAT12(fat)	((fat)->fat_type == 12)
#define FAT16(fat)	((fat)->fat_type == 16)

#define IS_EOFCL(fat, cl) \
	(((cl) & EOF_MASK) == ((fat)->fat_mask & EOF_MASK))

/*
 * File/directory node
 */
struct fatfs_node {
	struct fat_dirent dirent; /* copy of directory entry */
	u_long	sector;		/* sector# for directory entry */
	u_long	offset;		/* offset of directory entry in sector */
};

extern struct vnops fatfs_vnops;

/* Macro to convert cluster# to logical sector# */
#define cl_to_sec(fat, cl) \
            (fat->data_start + (cl - 2) * fat->sec_per_cl)

struct dirent {
	uint32_t d_fileno;		/* file number of entry */
	uint16_t d_reclen;		/* length of this record */
	uint16_t d_namlen;		/* length of string in d_name */
	uint8_t  d_type; 		/* file type, see below */
	char	 d_name[NAME_MAX];	/* name must be no longer than this */
};

/*
 * File types
 */
#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12
#define	DT_WHT		14

/*
 * Convert between stat structure types and directory types.
 */
#define	IFTODT(mode)	(((mode) & 0170000) >> 12)
#define	DTTOIF(dirtype)	((dirtype) << 12)

/*
* Vnode types.
*/
enum {
	VNON,	    /* no type */
	VREG,	    /* regular file  */
	VDIR,	    /* directory */
	VBLK,	    /* block device */
	VCHR,	    /* character device */
	VLNK,	    /* symbolic link */
	VSOCK,	    /* socks */
	VFIFO	    /* FIFO */
};

/*
* Reading or writing any of these items requires holding the
* appropriate lock.
*/
struct vnode {
	struct list	v_link;		/* link for hash list */
	struct mount	*v_mount;	/* mounted vfs pointer */
	int		v_refcnt;	/* reference count */
	int		v_type;		/* vnode type */
	int		v_flags;	/* vnode flag */
	mode_t		v_mode;		/* file mode */
	size_t		v_size;		/* file size */
	int		v_nrlocks;	/* lock count (for debug) */
	int		v_blkno;	/* block number */
	char		*v_path;	/* pointer to path in fs */
	void		*v_data;	/* private data for fs */
};
typedef struct vnode *vnode_t;

/* flags for vnode */
#define VROOT		0x0001		/* root of its file system */
#define VISTTY		0x0002		/* device is tty */
#define VPROTDEV	0x0004		/* protected device */

/*
* Vnode attribute
*/
struct vattr {
	int		va_type;	/* vnode type */
	mode_t		va_mode;	/* file access mode */
};

/*
*  Modes.
*/
#define	VREAD	00004		/* read, write, execute permissions */
#define	VWRITE	00002
#define	VEXEC	00001


/*
 * Mount data
 */
struct mount {
	struct list	m_link;		/* link to next mount point */
	int		m_flags;	/* mount flag */
	int		m_count;	/* reference count */
	char		m_path[PATH_MAX]; /* mounted path */
	dev_t		m_dev;		/* mounted device */
	struct vnode	*m_root;	/* root vnode */
	struct vnode	*m_covered;	/* vnode covered on parent fs */
	void		*m_data;	/* private data for fs */
};
typedef struct mount *mount_t;


/*
 * Mount flags.
 *
 * Unmount uses MNT_FORCE flag.
 */
#define	MNT_RDONLY	0x00000001	/* read only filesystem */
#define	MNT_SYNCHRONOUS	0x00000002	/* file system written synchronously */
#define	MNT_NOEXEC	0x00000004	/* can't exec from filesystem */
#define	MNT_NOSUID	0x00000008	/* don't honor setuid bits on fs */
#define	MNT_NODEV	0x00000010	/* don't interpret special files */
#define	MNT_UNION	0x00000020	/* union with underlying filesystem */
#define	MNT_ASYNC	0x00000040	/* file system written asynchronously */

/*
 * exported mount flags.
 */
#define	MNT_EXRDONLY	0x00000080	/* exported read only */
#define	MNT_EXPORTED	0x00000100	/* file system is exported */
#define	MNT_DEFEXPORTED	0x00000200	/* exported to the world */
#define	MNT_EXPORTANON	0x00000400	/* use anon uid mapping for everyone */
#define	MNT_EXKERB	0x00000800	/* exported with Kerberos uid mapping */

/*
 * Flags set by internal operations.
 */
#define	MNT_LOCAL	0x00001000	/* filesystem is stored locally */
#define	MNT_QUOTA	0x00002000	/* quotas are enabled on filesystem */
#define	MNT_ROOTFS	0x00004000	/* identifies the root filesystem */

/*
 * Mask of flags that are visible to statfs()
 */
#define	MNT_VISFLAGMASK	0x0000ffff

/*
 * Buffer header
 */
struct buf {
	int		b_flags;	/* see defines below */
	dev_t	b_dev;		/* device number */
	int		b_blkno;	/* block # on device */
	char	*b_data;	/* pointer to data buffer */
};

/*
 * These flags are kept in b_flags.
 */
#define	B_BUSY		0x00000001	/* I/O in progress. */
#define	B_DELWRI	0x00000002	/* delay I/O until buffer reused. */
#define	B_INVAL		0x00000004	/* does not contain valid info. */
#define	B_READ		0x00000008	/* read buffer. */
#define	B_DONE		0x00000010	/* I/O completed. */

/*
 * File structure
 */
struct file {
	int		f_flags;	/* open flag */
	int		f_count;	/* reference count */
	off_t		f_offset;	/* current position in file */
	struct vnode	*f_vnode;	/* vnode */
};
typedef struct file *file_t;




__BEGIN_DECLS
int	 fat_next_cluster(struct fatfsmount *fmp, u_long cl, u_long *next);
int	 fat_set_cluster(struct fatfsmount *fmp, u_long cl, u_long next);
int	 fat_alloc_cluster(struct fatfsmount *fmp, u_long scan_start, u_long *free);
int	 fat_free_clusters(struct fatfsmount *fmp, u_long start);
int	 fat_seek_cluster(struct fatfsmount *fmp, u_long start, u_long offset,
			    u_long *cl);
int	 fat_expand_file(struct fatfsmount *fmp, u_long cl, int size);
int	 fat_expand_dir(struct fatfsmount *fmp, u_long cl, u_long *new_cl);

void	 fat_convert_name(char *org, char *name);
void	 fat_restore_name(char *org, char *name);
int	 fat_valid_name(char *name);
int	 fat_compare_name(char *n1, char *n2);
void	 fat_mode_to_attr(mode_t mode, u_char *attr);
void	 fat_attr_to_mode(u_char attr, mode_t *mode);

int	 fatfs_lookup_node(vnode_t dvp, char *name, struct fatfs_node *node);
int	 fatfs_get_node(vnode_t dvp, int index, struct fatfs_node *node);
int	 fatfs_put_node(struct fatfsmount *fmp, struct fatfs_node *node);
int	 fatfs_add_node(vnode_t dvp, struct fatfs_node *node);
__END_DECLS

#endif /* !_FATFS_H */
