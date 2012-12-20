/**
 * @file
 * @brief
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */


#ifndef EXT_H_
#define EXT_H_

/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)fs.h	8.10 (Berkeley) 10/27/94
 *  Modified for ext2fs by Manuel Bouyer.
 */

/*
 * Copyright (c) 1997 Manuel Bouyer.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	@(#)fs.h	8.10 (Berkeley) 10/27/94
 *  Modified for ext2fs by Manuel Bouyer.
 */


#define SECTOR_SIZE 512
#define BBSIZE		1024
#define SBSIZE		1024
#define	BBOFF		((long)(0))
#define	SBOFF		((long)(BBOFF + BBSIZE))
/*#define	BBLOCK		((long long)(0))
#define	SBLOCK		((long long)(BBLOCK + BBSIZE / SECTOR_SIZE))
*/

/*
 * MINBSIZE is the smallest allowable block size.
 * MINBSIZE must be big enough to hold a cylinder group block,
 * thus changes to (struct cg) must keep its size within MINBSIZE.
 * Note that super blocks are always of size SBSIZE,
 * and that both SBSIZE and MAXBSIZE must be >= MINBSIZE.
 */
#define LOG_MINBSIZE	10
#define MINBSIZE	(1 << LOG_MINBSIZE)

/*
 * Filesystem identification
 */
#define	E2FS_MAGIC	0xef53	/* the ext2fs magic number */
#define E2FS_REV0	0	/* GOOD_OLD revision */
#define E2FS_REV1	1	/* Support compat/incompat features */

/* compatible/incompatible features */
#define EXT2F_COMPAT_PREALLOC		0x0001
#define EXT2F_COMPAT_HASJOURNAL		0x0004
#define EXT2F_COMPAT_RESIZE		    0x0010

#define EXT2F_ROCOMPAT_SPARSESUPER	0x0001
#define EXT2F_ROCOMPAT_LARGEFILE	0x0002
#define EXT2F_ROCOMPAT_BTREE_DIR	0x0004

#define EXT2F_INCOMPAT_COMP			0x0001
#define EXT2F_INCOMPAT_FTYPE		0x0002

/*
 * Features supported in this implementation
 *
 * We support the following REV1 features:
 * - EXT2F_ROCOMPAT_SPARSESUPER
 *    superblock backups stored only in cg_has_sb(bno) groups
 * - EXT2F_ROCOMPAT_LARGEFILE
 *    use e2di_dacl in struct ext2fs_dinode to store
 *    upper 32bit of size for >2GB files
 * - EXT2F_INCOMPAT_FTYPE
 *    store file type to e2d_type in struct ext2fs_direct
 *    (on REV0 e2d_namlen is uint16_t and no e2d_type, like ffs)
 */
#define EXT2F_COMPAT_SUPP		0x0000
#define EXT2F_ROCOMPAT_SUPP		(EXT2F_ROCOMPAT_SPARSESUPER \
					 | EXT2F_ROCOMPAT_LARGEFILE)
#define EXT2F_INCOMPAT_SUPP		EXT2F_INCOMPAT_FTYPE

/*
 * If the EXT2F_ROCOMPAT_SPARSESUPER flag is set, the cylinder group has a
 * copy of the super and cylinder group descriptors blocks only if it's
 * 1, a power of 3, 5 or 7
 */

/* EXT2FS metadatas are stored in little-endian byte order. These macros
 * helps reading theses metadatas
 */

//#if defined(__LITTLE_ENDIAN)
#	define h2fs16(x) (x)
#	define h2fs32(x) (x)
#	define h2fs64(x) (x)
#	define fs2h16(x) (x)
#	define fs2h32(x) (x)
#	define fs2h64(x) (x)
#	define e2fs_sbload(old, new) memcpy((new), (old), SBSIZE);
#	define e2fs_cgload(old, new, size) memcpy((new), (old), (size));
#	define e2fs_sbsave(old, new) memcpy((new), (old), SBSIZE);
#	define e2fs_cgsave(old, new, size) memcpy((new), (old), (size));
/*#else
void e2fs_sb_bswap(struct ext2fs *, struct ext2fs *);
void e2fs_cg_bswap(struct ext2_gd *, struct ext2_gd *, int);
#	define h2fs16(x) bswap16(x)
#	define h2fs32(x) bswap32(x)
#	define h2fs64(x) bswap64(x)
#	define fs2h16(x) bswap16(x)
#	define fs2h32(x) bswap32(x)
#	define fs2h64(x) bswap64(x)
#	define e2fs_sbload(old, new) e2fs_sb_bswap((old), (new))
#	define e2fs_cgload(old, new, size) e2fs_cg_bswap((old), (new), (size));
#	define e2fs_sbsave(old, new) e2fs_sb_bswap((old), (new))
#	define e2fs_cgsave(old, new, size) e2fs_cg_bswap((old), (new), (size));
#endif
*/
/*
 * Turn file system block numbers into disk block addresses.
 * This maps file system blocks to device size blocks.
 */
#define fsbtodb(fs, b)	((b) << (fs)->e2fs_fsbtodb)
#define dbtofsb(fs, b)	((b) >> (fs)->e2fs_fsbtodb)

/*
 * Macros for handling inode numbers:
 *	 inode number to file system block offset.
 *	 inode number to cylinder group number.
 *	 inode number to file system block address.
 */
#define	ino_to_cg(fs, x)	(((x) - 1) / (fs)->e2fs.e2fs_ipg)
#define	ino_to_fsba(fs, x)						\
	((fs)->e2fs_gd[ino_to_cg((fs), (x))].ext2bgd_i_tables +		\
	(((x) - 1) % (fs)->e2fs.e2fs_ipg) / (fs)->e2fs_ipb)
#define	ino_to_fsbo(fs, x)	(((x) - 1) % (fs)->e2fs_ipb)

/*
 * Give cylinder group number for a file system block.
 * Give cylinder group block number for a file system block.
 */
#define	dtog(fs, d) (((d) - (fs)->e2fs.e2fs_first_dblock) / (fs)->e2fs.e2fs_fpg)
#define	dtogd(fs, d) \
	(((d) - (fs)->e2fs.e2fs_first_dblock) % (fs)->e2fs.e2fs_fpg)

/*
 * The following macros optimize certain frequently calculated
 * quantities by using shifts and masks in place of divisions
 * modulos and multiplications.
 */
#define blkoff(fs, loc)		/* calculates (loc % fs->e2fs_bsize) */ \
	((loc) & (fs)->e2fs_qbmask)
#define lblktosize(fs, blk)	/* calculates (blk * fs->e2fs_bsize) */ \
	((blk) << (fs)->e2fs_bshift)
#define lblkno(fs, loc)		/* calculates (loc / fs->e2fs_bsize) */ \
	((loc) >> (fs)->e2fs_bshift)
#define blkroundup(fs, size)	/* calculates roundup(size, fs->e2fs_bsize) */ \
	(((size) + (fs)->e2fs_qbmask) & (fs)->e2fs_bmask)
#define fragroundup(fs, size)	/* calculates roundup(size, fs->e2fs_bsize) */ \
	(((size) + (fs)->e2fs_qbmask) & (fs)->e2fs_bmask)
/*
 * Determine the number of available frags given a
 * percentage to hold in reserve.
 */
#define freespace(fs) \
   ((fs)->e2fs.e2fs_fbcount - (fs)->e2fs.e2fs_rbcount)

/*
 * Number of indirects in a file system block.
 */
#define	NINDIR(fs)	((fs)->e2fs_bsize / sizeof(uint32_t))

#define NXADDR	2
#define	NDADDR	12			/* Direct addresses in inode. */
#define	NIADDR	3			/* Indirect addresses in inode. */

#define EXT2_MAXSYMLINKLEN ((NDADDR+NIADDR) * sizeof (uint32_t))

/*
 * The root inode is the root of the file system.  Inode 0 can't be used for
 * normal purposes and bad blocks are normally linked to inode 1, thus
 * the root inode is 2.
 * Inode 3 to 10 are reserved in ext2fs.
 */
#define	EXT2_BADBLKINO		((unsigned long)1)
#define	EXT2_ROOTINO		((unsigned long)2)
#define	EXT2_ACLIDXINO		((unsigned long)3)
#define	EXT2_ACLDATAINO		((unsigned long)4)
#define	EXT2_BOOTLOADERINO	((unsigned long)5)
#define	EXT2_UNDELDIRINO	((unsigned long)6)
#define	EXT2_RESIZEINO		((unsigned long)7)
#define	EXT2_JOURNALINO		((unsigned long)8)
#define	EXT2_FIRSTINO		((unsigned long)11)

/* File types. */
#define	EXT2_IFMT		0170000		/* Mask of file type. */
#define	EXT2_IFIFO		0010000		/* Named pipe (fifo). */
#define	EXT2_IFCHR		0020000		/* Character device. */
#define	EXT2_IFDIR		0040000		/* Directory file. */
#define	EXT2_IFBLK		0060000		/* Block device. */
#define	EXT2_IFREG		0100000		/* Regular file. */
#define	EXT2_IFLNK		0120000		/* Symbolic link. */
#define	EXT2_IFSOCK		0140000		/* UNIX domain socket. */

#define	MAXPATHLEN      	1024
#define	MAXSYMLINKS			32
#define	EXT2FS_MAXNAMLEN	255


/* Size of on-disk inode. */
#define EXT2_REV0_DINODE_SIZE	sizeof(struct ext2fs_dinode)
#define EXT2_DINODE_SIZE(fs)	((fs)->e2fs.e2fs_rev > E2FS_REV0 ?	\
				    (fs)->e2fs.e2fs_inode_size :	\
				    EXT2_REV0_DINODE_SIZE)

#	define e2fs_iload(old, new)	\
		memcpy((new),(old),sizeof(struct ext2fs_dinode))
#	define e2fs_isave(old, new)	\
		memcpy((new),(old),sizeof(struct ext2fs_dinode))

#define	howmany(x, y)	(((x)+((y)-1))/(y))

/*
 * Super block for an ext2fs file system.
 */
 struct ext2fs {
	uint32_t  e2fs_icount;		/* Inode count */
	uint32_t  e2fs_bcount;		/* blocks count */
	uint32_t  e2fs_rbcount;		/* reserved blocks count */
	uint32_t  e2fs_fbcount;		/* free blocks count */
	uint32_t  e2fs_ficount;		/* free inodes count */
	uint32_t  e2fs_first_dblock;	/* first data block */
	uint32_t  e2fs_log_bsize;	/* block size = 1024*(2^e2fs_log_bsize) */
	uint32_t  e2fs_fsize;		/* fragment size */
	uint32_t  e2fs_bpg;		/* blocks per group */
	uint32_t  e2fs_fpg;		/* frags per group */
	uint32_t  e2fs_ipg;		/* inodes per group */
	uint32_t  e2fs_mtime;		/* mount time */
	uint32_t  e2fs_wtime;		/* write time */
	uint16_t  e2fs_mnt_count;	/* mount count */
	uint16_t  e2fs_max_mnt_count;	/* max mount count */
	uint16_t  e2fs_magic;		/* magic number */
	uint16_t  e2fs_state;		/* file system state */
	uint16_t  e2fs_beh;		/* behavior on errors */
	uint16_t  e2fs_minrev;		/* minor revision level */
	uint32_t  e2fs_lastfsck;	/* time of last fsck */
	uint32_t  e2fs_fsckintv;	/* max time between fscks */
	uint32_t  e2fs_creator;		/* creator OS */
	uint32_t  e2fs_rev;		/* revision level */
	uint16_t  e2fs_ruid;		/* default uid for reserved blocks */
	uint16_t  e2fs_rgid;		/* default gid for reserved blocks */
	/* EXT2_DYNAMIC_REV superblocks */
	uint32_t  e2fs_first_ino;	/* first non-reserved inode */
	uint16_t  e2fs_inode_size;	/* size of inode structure */
	uint16_t  e2fs_block_group_nr;	/* block grp number of this sblk*/
	uint32_t  e2fs_features_compat;	/*  compatible feature set */
	uint32_t  e2fs_features_incompat; /* incompatible feature set */
	uint32_t  e2fs_features_rocompat; /* RO-compatible feature set */
	uint8_t   e2fs_uuid[16];	/* 128-bit uuid for volume */
	char      e2fs_vname[16];	/* volume name */
	char      e2fs_fsmnt[64];	/* name mounted on */
	uint32_t  e2fs_algo;		/* For compression */
	uint8_t   e2fs_prealloc;	/* # of blocks to preallocate */
	uint8_t   e2fs_dir_prealloc;	/* # of blocks to preallocate for dir */
	uint16_t  e2fs_reserved_ngdb;	/* # of reserved gd blocks for resize */
	uint32_t  reserved2[204];
};

/* ext2 file system block group descriptor */
struct ext2_gd {
	uint32_t ext2bgd_b_bitmap;	/* blocks bitmap block */
	uint32_t ext2bgd_i_bitmap;	/* inodes bitmap block */
	uint32_t ext2bgd_i_tables;	/* inodes table block  */
	uint16_t ext2bgd_nbfree;	/* number of free blocks */
	uint16_t ext2bgd_nifree;	/* number of free inodes */
	uint16_t ext2bgd_ndirs;		/* number of directories */
	uint16_t reserved;
	uint32_t reserved2[3];
};

/* ext2 file system directory descriptor */
struct	ext2fs_direct {
	uint32_t e2d_ino;		/* inode number of entry */
	uint16_t e2d_reclen;	/* length of this record */
	uint8_t e2d_namlen;		/* length of string in d_name */
	uint8_t e2d_type;		/* file type */
	char e2d_name[EXT2FS_MAXNAMLEN];/* name with length<=EXT2FS_MAXNAMLEN */
};

/*
 * To avoid having a lot of filesystem-block sized buffers lurking (which
 * could be 32k) we only keep a few entries of the indirect block map.
 * With 8k blocks, 2^8 blocks is ~500k so we reread the indirect block
 * ~13 times pulling in a 6M kernel.
 * The cache size must be smaller than the smallest filesystem block,
 * so LN2_IND_CACHE_SZ <= 9 (UFS2 and 4k blocks).
 */
#define LN2_IND_CACHE_SZ	6
#define IND_CACHE_SZ		(1 << LN2_IND_CACHE_SZ)
#define IND_CACHE_MASK		(IND_CACHE_SZ - 1)

struct ext2fs_dinode {
	uint16_t	e2di_mode;	/*   0: IFMT, permissions; see below. */
	uint16_t	e2di_uid;	/*   2: Owner UID */
	uint32_t	e2di_size;	/*	 4: Size (in bytes) */
	uint32_t	e2di_atime;	/*	 8: Acces time */
	uint32_t	e2di_ctime;	/*	12: Create time */
	uint32_t	e2di_mtime;	/*	16: Modification time */
	uint32_t	e2di_dtime;	/*	20: Deletion time */
	uint16_t	e2di_gid;	/*  24: Owner GID */
	uint16_t	e2di_nlink;	/*  26: File link count */
	uint32_t	e2di_nblock;	/*  28: Blocks count */
	uint32_t	e2di_flags;	/*  32: Status flags (chflags) */
	uint32_t	e2di_linux_reserved1; /* 36 */
	uint32_t	e2di_blocks[NDADDR+NIADDR]; /* 40: disk blocks */
	uint32_t	e2di_gen;	/* 100: generation number */
	uint32_t	e2di_facl;	/* 104: file ACL (not implemented) */
	uint32_t	e2di_dacl;	/* 108: dir ACL (not implemented) */
	uint32_t	e2di_faddr;	/* 112: fragment address */
	uint8_t		e2di_nfrag;	/* 116: fragment number */
	uint8_t		e2di_fsize;	/* 117: fragment size */
	uint16_t	e2di_linux_reserved2; /* 118 */
	uint16_t	e2di_uid_high;	/* 120: Owner UID top 16 bits */
	uint16_t	e2di_gid_high;	/* 122: Owner GID top 16 bits */
	uint32_t	e2di_linux_reserved3; /* 124 */
};

/* in-memory data for ext2fs */
typedef struct ext2_fs_info {
		char mntfrom[MAX_LENGTH_PATH_NAME];
		char mntto[MAX_LENGTH_PATH_NAME];
		struct ext2fs e2fs;
		int32_t	e2fs_bsize;	/* block size */
		int32_t e2fs_bshift;	/* ``lblkno'' calc of logical blkno */
		int32_t e2fs_bmask;	/* ``blkoff'' calc of blk offsets */
		int64_t e2fs_qbmask;	/* ~fs_bmask - for use with quad size */
		int32_t	e2fs_fsbtodb;	/* fsbtodb and dbtofsb shift constant */
		int32_t	e2fs_ncg;	/* number of cylinder groups */
		int32_t	e2fs_ngdb;	/* number of group descriptor block */
		int32_t	e2fs_ipb;	/* number of inodes per block */
		int32_t	e2fs_itpg;	/* number of inode table per group */
		struct	ext2_gd *e2fs_gd; /* group descripors */
} ext2_fs_info_t;

/*
 * In-core open file.
 */
typedef struct ext2_file_info {
	//struct ext2_fs_info	*f_fs;		/* pointer to super-block */
	struct ext2fs_dinode	f_di;		/* copy of on-disk inode */
	uint		f_nishift;	/* for blocks in indirect block */
	int32_t		f_ind_cache_block;
	int32_t		f_ind_cache[IND_CACHE_SZ];

	char		*f_buf;		/* buffer for data block */
	size_t		f_buf_size;	/* size of data block */
	int64_t		f_buf_blkno;	/* block number of data block */
	long		f_seekp;	/* local seek pointer */
} ext2_file_info_t;

#endif /* EXT_H_ */
