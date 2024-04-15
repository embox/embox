/**
 * @file
 * @brief ext4 driver
 *
 * @date 10.09.2013
 * @author Alexander Kalmuk
 */


#ifndef FS_EXT4_H_
#define FS_EXT4_H_

#include <linux/types.h>
#include <limits.h>

#define EXT4_NAME_LEN 255
#define EXT4_IND_CACHE_SZ 128

#define E4FS_MAGIC 0xef53	/* the ext4fs magic number */

#define EXT4_R_INODE 0
#define EXT4_W_INODE 1
#define EXT4_FIRSTINO 8

#define EXT4_INODE_SIZE(s) ((s)->s_inode_size)
#define EXT4_FIRST_INO(s) ((s)->s_first_ino)

#define EXT4_NDIR_BLOCKS        12
#define EXT4_IND_BLOCK          EXT4_NDIR_BLOCKS
#define EXT4_DIND_BLOCK         (EXT4_IND_BLOCK + 1)
#define EXT4_TIND_BLOCK         (EXT4_DIND_BLOCK + 1)
#define EXT4_N_BLOCKS           (EXT4_TIND_BLOCK + 1)

#define EXT4_DIR_ENTRY_ALIGN         4
/* ino + rec_len + name_len + file_type, doesn't include name and padding */
#define EXT4_MIN_DIR_ENTRY_SIZE	8

#define EXT4_DINODE_SIZE(fs) ((fs)->e4sb.s_inode_size)

#define EXT4_DIR_ENTRY_CONTENTS_SIZE(d) (EXT4_MIN_DIR_ENTRY_SIZE + (d)->name_len)
/* size with padding */
#define EXT4_DIR_ENTRY_ACTUAL_SIZE(d) (EXT4_DIR_ENTRY_CONTENTS_SIZE(d) + \
        ((EXT4_DIR_ENTRY_CONTENTS_SIZE(d) & 0x03) == 0 ? 0 : \
			EXT4_DIR_ENTRY_ALIGN - (EXT4_DIR_ENTRY_CONTENTS_SIZE(d) & 0x03) ))

/* How many bytes can be taken from the end of dentry */
#define EXT4_DIR_ENTRY_SHRINK(d)    ((d)->rec_len \
					- EXT4_DIR_ENTRY_ACTUAL_SIZE(d))

/* Dentry can have padding, which can be used to enlarge namelen */
#define EXT4_DIR_ENTRY_MAX_NAME_LEN(d)	((d)->rec_len \
						- EXT4_MIN_DIR_ENTRY_SIZE)

/* Current position in block */
#define EXT4_CUR_DISC_DIR_POS(cur_desc, base)  ((char*)cur_desc - (char*)base)
/* Return pointer to the next dentry */
#define EXT4_NEXT_DISC_DIR_DESC(cur_desc)	((struct ext4_dir*)\
					((char*)cur_desc + cur_desc->rec_len))
/* Return next dentry's position in block */
#define EXT4_NEXT_DISC_DIR_POS(cur_desc, base) (cur_desc->rec_len +\
					   EXT4_CUR_DISC_DIR_POS(cur_desc, base))

#define EXT4_FS_BITCHUNK_BITS (sizeof(uint32_t) * 8)

#define EXT4_NO_BLOCK ((uint32_t) 0)

struct ext4_super_block {
/*00*/	__le32	s_inodes_count;		/* Inodes count */
	__le32	s_blocks_count;	/* Blocks count */
	__le32	s_r_blocks_count;	/* Reserved blocks count */
	__le32	s_free_blocks_count;	/* Free blocks count */
/*10*/	__le32	s_free_inodes_count;	/* Free inodes count */
	__le32	s_first_data_block;	/* First Data Block */
	__le32	s_log_block_size;	/* Block size */
	__le32	s_log_cluster_size;	/* Allocation cluster size */
/*20*/	__le32	s_blocks_per_group;	/* # Blocks per group */
	__le32	s_clusters_per_group;	/* # Clusters per group */
	__le32	s_inodes_per_group;	/* # Inodes per group */
	__le32	s_mtime;		/* Mount time */
/*30*/	__le32	s_wtime;		/* Write time */
	__le16	s_mnt_count;		/* Mount count */
	__le16	s_max_mnt_count;	/* Maximal mount count */
	__le16	s_magic;		/* Magic signature */
	__le16	s_state;		/* File system state */
	__le16	s_errors;		/* Behaviour when detecting errors */
	__le16	s_minor_rev_level;	/* minor revision level */
/*40*/	__le32	s_lastcheck;		/* time of last check */
	__le32	s_checkinterval;	/* max. time between checks */
	__le32	s_creator_os;		/* OS */
	__le32	s_rev_level;		/* Revision level */
/*50*/	__le16	s_def_resuid;		/* Default uid for reserved blocks */
	__le16	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT4_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 *
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	__le32	s_first_ino;		/* First non-reserved inode */
	__le16  s_inode_size;		/* size of inode structure */
	__le16	s_block_group_nr;	/* block group # of this superblock */
	__le32	s_feature_compat;	/* compatible feature set */
/*60*/	__le32	s_feature_incompat;	/* incompatible feature set */
	__le32	s_feature_ro_compat;	/* readonly-compatible feature set */
/*68*/	__u8	s_uuid[16];		/* 128-bit uuid for volume */
/*78*/	char	s_volume_name[16];	/* volume name */
/*88*/	char	s_last_mounted[64];	/* directory where last mounted */
/*C8*/	__le32	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	__u8	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	__u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__le16	s_reserved_gdt_blocks;	/* Per group desc for online growth */
	/*
	 * Journaling support valid if EXT4_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
/*D0*/	__u8	s_journal_uuid[16];	/* uuid of journal superblock */
/*E0*/	__le32	s_journal_inum;		/* inode number of journal file */
	__le32	s_journal_dev;		/* device number of journal file */
	__le32	s_last_orphan;		/* start of list of inodes to delete */
	__le32	s_hash_seed[4];		/* HTREE hash seed */
	__u8	s_def_hash_version;	/* Default hash version to use */
	__u8	s_jnl_backup_type;
	__le16  s_desc_size;		/* size of group descriptor */
/*100*/	__le32	s_default_mount_opts;
	__le32	s_first_meta_bg;	/* First metablock block group */
	__le32	s_mkfs_time;		/* When the filesystem was created */
	__le32	s_jnl_blocks[17];	/* Backup of the journal inode */
	/* 64bit support valid if EXT4_FEATURE_COMPAT_64BIT */
/*150*/	__le32	s_blocks_count_hi;	/* Blocks count */
	__le32	s_r_blocks_count_hi;	/* Reserved blocks count */
	__le32	s_free_blocks_count_hi;	/* Free blocks count */
	__le16	s_min_extra_isize;	/* All inodes have at least # bytes */
	__le16	s_want_extra_isize; 	/* New inodes should reserve # bytes */
	__le32	s_flags;		/* Miscellaneous flags */
	__le16  s_raid_stride;		/* RAID stride */
	__le16  s_mmp_update_interval;  /* # seconds to wait in MMP checking */
	__le64  s_mmp_block;            /* Block for multi-mount protection */
	__le32  s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
	__u8	s_log_groups_per_flex;  /* FLEX_BG group size */
	__u8	s_reserved_char_pad;
	__le16  s_reserved_pad;
	__le64	s_kbytes_written;	/* nr of lifetime kilobytes written */
	__le32	s_snapshot_inum;	/* Inode number of active snapshot */
	__le32	s_snapshot_id;		/* sequential ID of active snapshot */
	__le64	s_snapshot_r_blocks_count; /* reserved blocks for active
					      snapshot's future use */
	__le32	s_snapshot_list;	/* inode number of the head of the
					   on-disk snapshot list */

	__le32	s_error_count;		/* number of fs errors */
	__le32	s_first_error_time;	/* first time an error happened */
	__le32	s_first_error_ino;	/* inode involved in first error */
	__le64	s_first_error_block;	/* block involved of first error */
	__u8	s_first_error_func[32];	/* function where the error happened */
	__le32	s_first_error_line;	/* line number where error happened */
	__le32	s_last_error_time;	/* most recent time of an error */
	__le32	s_last_error_ino;	/* inode involved in last error */
	__le32	s_last_error_line;	/* line number where error happened */
	__le64	s_last_error_block;	/* block involved of last error */
	__u8	s_last_error_func[32];	/* function where the error happened */

	__u8	s_mount_opts[64];
	__le32	s_usr_quota_inum;	/* inode for tracking user quota */
	__le32	s_grp_quota_inum;	/* inode for tracking group quota */
	__le32	s_overhead_clusters;	/* overhead blocks/clusters in fs */
	__le32  s_reserved[109];        /* Padding to the end of the block */
};

struct ext4_group_desc {
	__le32	block_bitmap_lo;	/* Blocks bitmap block */
	__le32	inode_bitmap_lo;	/* Inodes bitmap block */
	__le32	inode_table_lo;	/* Inodes table block */
	__le16	free_blocks_count_lo;/* Free blocks count */
	__le16	free_inodes_count_lo;/* Free inodes count */
	__le16	used_dirs_count_lo;	/* Directories count */
	__le16	flags;		/* EXT4_BG_flags (INODE_UNINIT, etc) */
	__u32	reserved[2];		/* Likely block/inode bitmap checksum */
	__le16  itable_unused_lo;	/* Unused inodes count */
	__le16  checksum;		/* crc16(sb_uuid+group+desc) */
	__le32	block_bitmap_hi;	/* Blocks bitmap block MSB */
	__le32	inode_bitmap_hi;	/* Inodes bitmap block MSB */
	__le32	inode_table_hi;	/* Inodes table block MSB */
	__le16	free_blocks_count_hi;/* Free blocks count MSB */
	__le16	free_inodes_count_hi;/* Free inodes count MSB */
	__le16	used_dirs_count_hi;	/* Directories count MSB */
	__le16  itable_unused_hi;    /* Unused inodes count MSB */
	__u32	reserved2[3];
};

/* in-memory data for ext4fs */
typedef struct ext4_fs_info {
	struct ext4_super_block e4sb;
	struct ext4_group_desc *e4fs_gd; /* group descripors */
	/* The following items are only used when the super_block is in memory. */
	int32_t s_bshift;	/* ``lblkno'' calc of logical blkno */
	int32_t s_bmask;	/* ``blkoff'' calc of blk offsets */
	int64_t s_qbmask;	/* ~fs_bmask - for use with quad size */
	int32_t	s_fsbtodb;	/* fsbtodb and dbtofsb shift constant */
	int32_t	s_ncg;	/* number of cylinder groups */
	uint32_t   s_block_size;           /* block size in bytes. */
	uint32_t   s_inodes_per_block;     /* Number of inodes per block */
	uint32_t   s_itb_per_group;        /* Number of inode table blocks per group */
	uint32_t   s_gdb_count;            /* Number of group descriptor blocks */
	uint32_t   s_desc_per_block;       /* Number of group descriptors per block */
	uint32_t   s_groups_count;         /* Number of groups in the fs */
	size_t     s_page_count;		   /* Number of pages of embox for file r/w buffer*/
	u16_t      s_sectors_in_block;     /* s_block_size / 512 */
	uint32_t   s_bsearch;	           /* all data blocks  below this block are in use*/
	u8_t       s_blocksize_bits;       /* Used to calculate offsets (e.g. inode block),
								        * always s_log_block_size + 10.
									    */
	journal_t *journal;
	char mntto[PATH_MAX];
} ext4_fs_info_t;

/* ext2 file system directory descriptor */
struct ext4_dir {
	uint32_t inode;		/* Inode number */
	uint16_t rec_len;	/* Directory entry length */
	uint8_t name_len;		/* Length of string in d_name */
	uint8_t type;		/* File type */
	char name[EXT4_NAME_LEN];/* name with length<=EXT4FS_MAXNAMLEN */
};

/*
 * Structure of an inode on the disk
 */
struct ext4_inode {
	__le16	i_mode;		/* File mode */
	__le16	i_uid;		/* Low 16 bits of Owner Uid */
	__le32	i_size_lo;	/* Size in bytes */
	__le32	i_atime;	/* Access time */
	__le32	i_ctime;	/* Inode Change time */
	__le32	i_mtime;	/* Modification time */
	__le32	i_dtime;	/* Deletion Time */
	__le16	i_gid;		/* Low 16 bits of Group Id */
	__le16	i_links_count;	/* Links count */
	__le32	i_blocks_lo;	/* Blocks count */
	__le32	i_flags;	/* File flags */
	union {
		struct {
			__le32  l_i_version;
		} linux1;
		struct {
			__u32  h_i_translator;
		} hurd1;
		struct {
			__u32  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__le32	i_block[EXT4_N_BLOCKS];/* Pointers to blocks */
	__le32	i_generation;	/* File version (for NFS) */
	__le32	i_file_acl_lo;	/* File ACL */
	__le32	i_size_hi;
	__le32	i_obso_faddr;	/* Obsoleted fragment address */
	union {
		struct {
			__le16	l_i_blocks_high; /* were l_i_reserved1 */
			__le16	l_i_file_acl_high;
			__le16	l_i_uid_high;	/* these 2 fields */
			__le16	l_i_gid_high;	/* were reserved2[0] */
			__u32	l_i_reserved2;
		} linux2;
		struct {
			__le16	h_i_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			__u16	h_i_mode_high;
			__u16	h_i_uid_high;
			__u16	h_i_gid_high;
			__u32	h_i_author;
		} hurd2;
		struct {
			__le16	h_i_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			__le16	m_i_file_acl_high;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
	__le16	i_extra_isize;
	__le16	i_pad1;
	__le32  i_ctime_extra;  /* extra Change time      (nsec << 2 | epoch) */
	__le32  i_mtime_extra;  /* extra Modification time(nsec << 2 | epoch) */
	__le32  i_atime_extra;  /* extra Access time      (nsec << 2 | epoch) */
	__le32  i_crtime;       /* File Creation time */
	__le32  i_crtime_extra; /* extra FileCreationtime (nsec << 2 | epoch) */
	__le32  i_version_hi;	/* high 32 bits for 64-bit version */
};

/*
 * In-core open file.
 */
typedef struct ext4_file_info {
	struct ext4_inode f_di;		/* copy of on-disk inode */
	uint		f_nishift;	/* for blocks in indirect block */
	int32_t		f_ind_cache_block;
	int32_t		f_ind_cache[EXT4_IND_CACHE_SZ];

	char		*f_buf;		/* buffer for data block */
	size_t		f_buf_size;	/* size of data block */
	int64_t		f_buf_blkno;/* block number of data block */
	long		f_pointer;	/* local seek pointer */

	ino_t f_num;                /* inode number on its (minor) device */
	uint32_t f_bsearch;         /* where to start search for new blocks,
								 * also this is last allocated block.
								 */
	long f_last_pos_bl_alloc;
								/* last write position for which we allocated
								 * a new block (should be block i_bsearch).
								 * used to check for sequential operation.
								 */
} ext4_file_info_t;

/*
 * This is the extent on-disk structure.
 * It's used at the bottom of the tree.
 */
struct ext4_extent {
        __le32  ee_block;       /* first logical block extent covers */
        __le16  ee_len;         /* number of blocks covered by extent */
        __le16  ee_start_hi;    /* high 16 bits of physical block */
        __le32  ee_start_lo;    /* low 32 bits of physical block */
};

/*
 * This is index on-disk structure.
 * It's used at all the levels except the bottom.
 */
struct ext4_extent_idx {
        __le32  ei_block;       /* index covers logical blocks from 'block' */
        __le32  ei_leaf_lo;     /* pointer to the physical block of the next *
                                 * level. leaf or next index could be there */
        __le16  ei_leaf_hi;     /* high 16 bits of physical block */
        __u16   ei_unused;
};

/*
 * Each block (leaves and indexes), even inode-stored has header.
 */
struct ext4_extent_header {
        __le16  eh_magic;       /* probably will support different formats */
        __le16  eh_entries;     /* number of valid entries */
        __le16  eh_max;         /* capacity of store in entries */
        __le16  eh_depth;       /* has tree real underlying blocks? */
        __le32  eh_generation;  /* generation of the tree */
};

//(((uint64_t)(fs)->e4fs_gd[ext4_ino_to_cg((fs), (x))].inode_table_hi) >> 32)) +

#define	ext4_ino_to_cg(fs, x)	(((x) - 1) / (fs)->e4sb.s_inodes_per_group)
#define	ext4_ino_to_fsba(fs, x) \
		((fs)->e4fs_gd[ext4_ino_to_cg((fs), (x))].inode_table_lo +		\
		(((x) - 1) % (fs)->e4sb.s_inodes_per_group) / (fs)->s_inodes_per_block)
#define	ext4_ino_to_fsbo(fs, x)	(((x) - 1) % (fs)->s_inodes_per_block)

/* FIXME */
#define __ext4_lo_hi(lo, hi) (lo + ((uint64_t)hi >> 32))
#define ext4_inc_lo_hi(lo, hi) lo++
#define ext4_dec_lo_hi(lo, hi) lo--
#define ext4_add_lo_hi(lo, hi, add) lo += add
#define ext4_sub_lo_hi(lo, hi, sub) lo -= add

/* FIXME add inode.i_size_hi */
#define ext4_file_size(inode) inode.i_size_lo
#define ext4_free_inodes_count(group_desc) \
		__ext4_lo_hi(group_desc->free_inodes_count_lo, group_desc->free_inodes_count_hi)
#define ext4_free_blocks_count(group_desc) \
		__ext4_lo_hi(group_desc->free_blocks_count_lo, group_desc->free_blocks_count_hi)
#define ext4_inode_bitmap_len(group_desc) \
		__ext4_lo_hi(group_desc->inode_bitmap_lo, group_desc->inode_bitmap_hi)
#define ext4_block_bitmap_len(group_desc) \
		__ext4_lo_hi(group_desc->block_bitmap_lo, group_desc->block_bitmap_hi)
#define ext4_inode_table_len(group_desc) \
		__ext4_lo_hi(group_desc->inode_table_lo, group_desc->inode_table_hi)

#define e4fs_iload(old, new)	\
		memcpy((new),(old),sizeof(struct ext4_inode))
#define e4fs_isave(old, new)	\
		memcpy((new),(old),sizeof(struct ext4_inode))

extern uint32_t ext4_alloc_block(struct inode *node, uint32_t goal);
extern void ext4_free_block(struct inode *node, uint32_t bit);
extern struct ext4_group_desc* ext4_get_group_desc(unsigned int bnum, struct ext4_fs_info *fsi);
extern uint32_t ext4_setbit(uint32_t *bitmap, uint32_t max_bits, unsigned int word);
extern int ext4_unsetbit(uint32_t *bitmap, uint32_t bit);

#endif /* FS_EXT4_H_ */
