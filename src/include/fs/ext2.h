/**
 * @file
 * @brief
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */


#ifndef EXT_H_
#define EXT_H_
#include <mem/page.h>
#include <fs/ext2_super.h>
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


#define NO_BLOCK		((uint32_t) 0)
#define ROOT_INODE         2   /* inode number for root directory */
#define BOOT_BLOCK       0 /* block number of boot block */
#define START_BLOCK      2 /* first block of FS (not counting SB) */
#define BLOCK_ADDRESS_BYTES	4     /* bytes per address */

#define EXT2_NDIR_BLOCKS        12
#define EXT2_IND_BLOCK          EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK         (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK         (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS           (EXT2_TIND_BLOCK + 1)

#define WMAP_FREE           (1 << 0)

#define IN_CLEAN              0    /* inode disk and memory copies identical */
#define IN_DIRTY              1    /* inode disk and memory copies differ */
#define ATIME            002    /* set if atime field needs updating */
#define CTIME            004    /* set if ctime field needs updating */
#define MTIME            010    /* set if mtime field needs updating */

/* Miscellaneous constants */
#define SU_UID          0     /* super_user's uid_t */
#define NORMAL          0     /* forces get_block to do disk read */
#define NO_READ         1     /* prevents get_block from doing disk read */
#define PREFETCH        2     /* tells get_block not to read or mark dev */

#define INODE_BLOCK        0                             /* inode block */
#define DIRECTORY_BLOCK    1                             /* directory block */
#define INDIRECT_BLOCK     2                             /* pointer block */
#define MAP_BLOCK          3                             /* bit map */
#define FULL_DATA_BLOCK    5                             /* data, fully used */
#define PARTIAL_DATA_BLOCK 6                             /* data, partly used*/

#define EXT2_PREALLOC_BLOCKS		8
#define NR_INODES        512
#define CHAR_BIT 8

#define usizeof(t) ((unsigned) sizeof(t))

#define FS_BITMAP_CHUNKS(b) ((b)/usizeof (uint32_t))/* # map chunks/blk   */
#define FS_BITCHUNK_BITS		(usizeof(uint32_t) * CHAR_BIT)
#define FS_BITS_PER_BLOCK(b)	(FS_BITMAP_CHUNKS(b) * FS_BITCHUNK_BITS)

#define DIRTY 1

struct inode {
    u16_t  i_mode;         /* File mode */
    u16_t  i_uid;          /* Low 16 bits of Owner Uid */
    u32_t  i_size;         /* Size in bytes */
    u32_t  i_atime;        /* Access time */
    u32_t  i_ctime;        /* Creation time */
    u32_t  i_mtime;        /* Modification time */
    u32_t  i_dtime;        /* Deletion Time */
    u16_t  i_gid;          /* Low 16 bits of Group Id */
    u16_t  i_links_count;  /* Links count */
    u32_t  i_blocks;       /* 512-byte blocks count */
    u32_t  i_flags;        /* File flags */
    union {
        struct {
                u32_t  l_i_reserved1;
        } linux1;
        struct {
                u32_t  h_i_translator;
        } hurd1;
        struct {
                u32_t  m_i_reserved1;
        } masix1;
    } osd1;                         /* OS dependent 1 */
    u32_t  i_block[EXT2_N_BLOCKS];  /* Pointers to blocks */
    u32_t  i_generation;            /* File version (for NFS) */
    u32_t  i_file_acl;              /* File ACL */
    u32_t  i_dir_acl;               /* Directory ACL */
    u32_t  i_faddr;                 /* Fragment address */
    /*union {
        struct {
            u8_t    l_i_frag;       // Fragment number /
            u8_t    l_i_fsize;      // Fragment size /
            u16_t   i_pad1;
            u16_t  l_i_uid_high;   // these 2 fields    /
            u16_t  l_i_gid_high;   // were reserved2[0] /
            u32_t   l_i_reserved2;
        } linux2;
        struct {
            u8_t    h_i_frag;       // Fragment number /
            u8_t    h_i_fsize;      // Fragment size /
            u16_t  h_i_mode_high;
            u16_t  h_i_uid_high;
            u16_t  h_i_gid_high;
            u32_t  h_i_author;
        } hurd2;
        struct {
            u8_t    m_i_frag;       // Fragment number /
            u8_t    m_i_fsize;      // Fragment size /
            u16_t   m_pad1;
            u32_t   m_i_reserved2[2];
        } masix2;
    } osd2;                         // OS dependent 2 /
*/
    /* The following items are not present on the disk. */
    dev_t i_dev;                /* which device is the inode on */
    ino_t i_num;                /* inode number on its (minor) device */
    int i_count;                /* # times inode used; 0 means slot is free */
    struct super_block *i_sp;   /* pointer to super block for inode's device */
    char i_dirt;                /* CLEAN or DIRTY */
    uint32_t i_bsearch;          /* where to start search for new blocks,
                                 * also this is last allocated block.
				 */
    long i_last_pos_bl_alloc;  /* last write position for which we allocated
                                 * a new block (should be block i_bsearch).
				 * used to check for sequential operation.
				 */
    long i_last_dpos;          /* where to start dentry search */
    int i_last_dentry_size;	/* size of last found dentry */

    char i_mountpoint;          /* true if mounted on */

    char i_seek;                /* set on LSEEK, cleared on READ/WRITE */
    char i_update;              /* the ATIME, CTIME, and MTIME bits are here */

    uint32_t i_prealloc_blocks[EXT2_PREALLOC_BLOCKS];	/* preallocated blocks */
    int i_prealloc_count;	/* number of preallocated blocks */
    int i_prealloc_index;	/* index into i_prealloc_blocks */
    int i_preallocation;	/* use preallocation for this inode, normally
				 * it's reset only when non-sequential write
				 * happens.
				 */

};// inode[100];

struct buf {
  /* Data portion of the buffer. */
  void *data;

  /* Header portion of the buffer - internal to libminixfs. */
  struct buf *lmfs_next;       /* used to link all free bufs in a chain */
  struct buf *lmfs_prev;       /* used to link all free bufs the other way */
  struct buf *lmfs_hash;       /* used to link bufs on hash chains */
  uint32_t lmfs_blocknr;        /* block number of its (minor) device */
  dev_t lmfs_dev;              /* major | minor device where block resides */
  char lmfs_dirt;              /* BP_CLEAN or BP_DIRTY */
  char lmfs_count;             /* number of users of this buffer */
  unsigned int lmfs_bytes;     /* Number of bytes allocated in bp */
};

union fsdata_u {
    char b__data[PAGE_SIZE()];             /* ordinary user data */
/* indirect block */
    uint32_t b__ind[PAGE_SIZE()/sizeof(uint32_t)];
/* bit map block */
    uint32_t b__bitmap[FS_BITMAP_CHUNKS(PAGE_SIZE())];
};

/* A block is free if b_dev == NO_DEV. */

/* These defs make it possible to use to bp->b_data instead of bp->b.b__data */
#define b_data(bp)   ((union fsdata_u *) bp->data)->b__data
#define b_ind(bp) ((union fsdata_u *) bp->data)->b__ind
#define b_bitmap(bp) ((union fsdata_u *) bp->data)->b__bitmap

/* balloc.c */
void discard_preallocated_blocks(struct nas *nas, struct inode *rip);
uint32_t alloc_block(struct nas *nas, struct inode *rip, uint32_t goal);
void free_block(struct nas *nas, struct super_block *sp, uint32_t bit);

int ext2_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector);
int ext2_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector);

/* ialloc.c
struct inode *alloc_inode(struct inode *parent, mode_t bits);
void free_inode(struct inode *rip);*/

#endif /* EXT_H_ */
