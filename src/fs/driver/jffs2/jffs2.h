/**
 * @file
 * @brief
 *
 * @date 25.07.2013
 * @author Andrey Gazukin
 */

/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2002-2003 Free Software Foundation, Inc.
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: os-ecos.h,v 1.24 2005/02/09 09:23:55 pavlov Exp $
 *
 */

#ifndef __JFFS2_H__
#define __JFFS2_H__

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <asm/bug.h>
#include <asm/semaphore.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/time/clock_source.h>
#include <kernel/thread.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/stat.h>
#include <linux/compiler.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/rwsem.h>
#include <linux/version.h>
#include <linux/rbtree.h>

#define JFFS2_SUPER_MAGIC 0x72b6

/* Values we may expect to find in the 'magic' field */
#define JFFS2_OLD_MAGIC_BITMASK 0x1984
#define JFFS2_MAGIC_BITMASK 0x1985
#define KSAMTIB_CIGAM_2SFFJ 0x8519 /* For detecting wrong-endian fs */
#define JFFS2_EMPTY_BITMASK 0xffff
#define JFFS2_DIRTY_BITMASK 0x0000

/* We only allow a single char for length, and 0xFF is empty flash so
   we don't want it confused with a real length. Hence max 254.
*/
#define JFFS2_MAX_NAME_LEN 254

/* How small can we sensibly write nodes? */
#define JFFS2_MIN_DATA_LEN 128

#define JFFS2_COMPR_NONE	0x00
#define JFFS2_COMPR_ZERO	0x01
#define JFFS2_COMPR_RTIME	0x02
#define JFFS2_COMPR_RUBINMIPS	0x03
#define JFFS2_COMPR_COPY	0x04
#define JFFS2_COMPR_DYNRUBIN	0x05
#define JFFS2_COMPR_ZLIB	0x06
/* Compatibility flags. */
#define JFFS2_COMPAT_MASK 0xc000      /* What do to if an unknown nodetype is found */
#define JFFS2_NODE_ACCURATE 0x2000
/* INCOMPAT: Fail to mount the filesystem */
#define JFFS2_FEATURE_INCOMPAT 0xc000
/* ROCOMPAT: Mount read-only */
#define JFFS2_FEATURE_ROCOMPAT 0x8000
/* RWCOMPAT_COPY: Mount read/write, and copy the node when it's GC'd */
#define JFFS2_FEATURE_RWCOMPAT_COPY 0x4000
/* RWCOMPAT_DELETE: Mount read/write, and delete the node when it's GC'd */
#define JFFS2_FEATURE_RWCOMPAT_DELETE 0x0000

#define JFFS2_NODETYPE_DIRENT (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 1)
#define JFFS2_NODETYPE_INODE (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 2)
#define JFFS2_NODETYPE_CLEANMARKER (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 3)
#define JFFS2_NODETYPE_PADDING (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 4)

#define JFFS2_INO_FLAG_PREREAD	  1	/* Do read_inode() for this one at
					   mount time, don't wait for it to
					   happen later */
#define JFFS2_INO_FLAG_USERCOMPR  2	/* User has requested a specific
					   compression type */

/* These can go once we've made sure we've caught all uses without
   byteswapping */
typedef struct {
	uint32_t v32;
} __attribute__((packed))  jint32_t;

typedef struct {
	uint32_t m;
} __attribute__((packed))  jmode_t;

typedef struct {
	uint16_t v16;
} __attribute__((packed)) jint16_t;

struct jffs2_unknown_node {
	/* All start like this */
	jint16_t magic;
	jint16_t nodetype;
	jint32_t totlen; /* So we can skip over nodes we don't grok */
	jint32_t hdr_crc;
} __attribute__((packed));

struct jffs2_raw_dirent {
	jint16_t magic;
	jint16_t nodetype;	/* == JFFS2_NODETYPE_DIRENT */
	jint32_t totlen;
	jint32_t hdr_crc;
	jint32_t pino;
	jint32_t version;
	jint32_t ino; /* == zero for unlink */
	jint32_t mctime;
	uint8_t nsize;
	uint8_t type;
	uint8_t unused[2];
	jint32_t node_crc;
	jint32_t name_crc;
	uint8_t name[0];
} __attribute__((packed));

/* The JFFS2 raw inode structure: Used for storage on physical media.  */
/* The uid, gid, atime, mtime and ctime members could be longer, but
 * are left like this for space efficiency. If and when people decide
 * they really need them extended, it's simple enough to add support for
 * a new type of raw node.
 */
struct jffs2_raw_inode {
	jint16_t magic;      /* A constant magic number.  */
	jint16_t nodetype;   /* == JFFS2_NODETYPE_INODE */
	jint32_t totlen;     /* Total length of this node (inc data, etc.) */
	jint32_t hdr_crc;
	jint32_t ino;        /* Inode number.  */
	jint32_t version;    /* Version number.  */
	jmode_t mode;       /* The file's type or mode.  */
	jint16_t uid;        /* The file's owner.  */
	jint16_t gid;        /* The file's group.  */
	jint32_t isize;      /* Total resultant size of this inode (used for truncations)  */
	jint32_t atime;      /* Last access time.  */
	jint32_t mtime;      /* Last modification time.  */
	jint32_t ctime;      /* Change time.  */
	jint32_t offset;     /* Where to begin to write.  */
	jint32_t csize;      /* (Compressed) data size */
	jint32_t dsize;	     /* Size of the node's data. (after decompression) */
	uint8_t compr;       /* Compression algorithm used */
	uint8_t usercompr;   /* Compression algorithm requested by the user */
	jint16_t flags;	     /* See JFFS2_INO_FLAG_* */
	jint32_t data_crc;   /* CRC for the (compressed) data.  */
	jint32_t node_crc;   /* CRC for the raw inode (excluding data)  */
	uint8_t data[0];
} __attribute__((packed));

union jffs2_node_union {
	struct jffs2_raw_inode i;
	struct jffs2_raw_dirent d;
	struct jffs2_unknown_node u;
};

#define JFFS2_SB_FLAG_RO 1
#define JFFS2_SB_FLAG_SCANNING 2 /* Flash scanning is in progress */
#define JFFS2_SB_FLAG_BUILDING 4 /* File system building is in progress */

struct jffs2_inodirty;

/* A struct for the overall file system control.  Pointers to
 * jffs2_sb_info structs are named `c' in the source code.
 * Nee jffs_control
 */
struct jffs2_sb_info {
	uint32_t highest_ino;
	uint32_t checked_ino;

	unsigned int flags;

	struct task_struct *gc_task;	/* GC task struct */
	struct completion gc_thread_start; /* GC thread start completion */
	struct completion gc_thread_exit; /* GC thread exit completion port */

	struct semaphore alloc_sem;	/* Used to protect all the following
					   fields, and also to protect against
					   out-of-order writing of nodes. And GC. */
	uint32_t cleanmarker_size;	/* Size of an _inline_ CLEANMARKER
	                             * (i.e. zero for OOB CLEANMARKER
	                             */
	uint32_t flash_size;
	uint32_t used_size;
	uint32_t dirty_size;
	uint32_t wasted_size;
	uint32_t free_size;
	uint32_t erasing_size;
	uint32_t bad_size;
	uint32_t sector_size;
	uint32_t unchecked_size;

	uint32_t nr_free_blocks;
	uint32_t nr_erasing_blocks;

	/* Number of free blocks there must be before we... */
	uint8_t resv_blocks_write;	/* ... allow a normal filesystem write */
	uint8_t resv_blocks_deletion;	/* ... allow a normal filesystem deletion */
	uint8_t resv_blocks_gctrigger;	/* ... wake up the GC thread */
	uint8_t resv_blocks_gcbad;	/* ... pick a block from the bad_list to GC */
	uint8_t resv_blocks_gcmerge;	/* ... merge pages when garbage collecting */

	uint32_t nospc_dirty_size;

	uint32_t nr_blocks;
	struct jffs2_eraseblock *blocks;	/* The whole array of blocks. Used for getting blocks
						 * from the offset (blocks[ofs / sector_size]) */
	struct jffs2_eraseblock *nextblock;	/* The block we're currently filling */

	struct jffs2_eraseblock *gcblock;	/* The block we're currently garbage-collecting */

	struct list_head clean_list;		/* Blocks 100% full of clean data */
	struct list_head very_dirty_list;	/* Blocks with lots of dirty space */
	struct list_head dirty_list;		/* Blocks with some dirty space */
	struct list_head erasable_list;		/* Blocks which are completely dirty, and need erasing */
	struct list_head erasable_pending_wbuf_list;	/* Blocks which need erasing but only after the current wbuf is flushed */
	struct list_head erasing_list;		/* Blocks which are currently erasing */
	struct list_head erase_pending_list;	/* Blocks which need erasing now */
	struct list_head erase_complete_list;	/* Blocks which are erased and need the clean marker written to them */
	struct list_head free_list;		/* Blocks which are free and ready to be used */
	struct list_head bad_list;		/* Bad blocks. */
	struct list_head bad_used_list;		/* Bad blocks with valid data in. */

	spinlock_t erase_completion_lock;	/* Protect free_list and erasing_list
						   against erase completion handler */
	wait_queue_head_t erase_wait;		/* For waiting for erases to complete */

	wait_queue_head_t inocache_wq;
	struct jffs2_inode_cache **inocache_list;
	spinlock_t inocache_lock;

	/* Sem to allow jffs2_garbage_collect_deletion_dirent to
	   drop the erase_completion_lock while it's holding a pointer
	   to an obsoleted node. I don't like this. Alternatives welcomed. */
	struct semaphore erase_free_sem;

#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
	/* Write-behind buffer for NAND flash */
	unsigned char *wbuf;
	uint32_t wbuf_ofs;
	uint32_t wbuf_len;
	uint32_t wbuf_pagesize;
	struct jffs2_inodirty *wbuf_inodes;

	struct rw_semaphore wbuf_sem;	/* Protects the write buffer */

	/* Information about out-of-band area usage... */
	struct nand_oobinfo *oobinfo;
	uint32_t badblock_pos;
	uint32_t fsdata_pos;
	uint32_t fsdata_len;
#endif

};

struct jffs2_inode_info {
	/* We need an internal semaphore similar to inode->i_sem.
	 * Unfortunately, we can't used the existing one, because
	 * either the GC would deadlock, or we'd have to release it
	 * before letting GC proceed. Or we'd have to put ugliness
	 * into the GC code so it didn't attempt to obtain the i_sem
	 * for the inode(s) which are already locked
	 */
	struct semaphore sem;

	/* The highest (datanode) version number used for this ino */
	uint32_t highest_version;

	/* List of data fragments which make up the file */
	struct rb_root fragtree;

	/* There may be one datanode which isn't referenced by any of the
	 * above fragments, if it contains a metadata update but no actual
	 * data - or if this is a directory inode
	 */
	/* This also holds the _only_ dnode for symlinks/device nodes,
	 * etc.
	 */
	struct jffs2_full_dnode *metadata;

	/* Directory entries */
	struct jffs2_full_dirent *dents;

	/* The target path if this is the inode of a symlink */
	unsigned char *target;

	/* Some stuff we just have to keep in-core at all times, for each inode. */
	struct jffs2_inode_cache *inocache;

	uint16_t flags;
	uint8_t usercompr;
};

#define timestamp ktime_get_timeseconds
#define get_seconds clock_sys_ticks

#define CONFIG_JFFS2_ZLIB 1
//#define CYGOPT_FS_JFFS2_GCTHREAD 0
#define CYGNUM_JFFS2_GC_THREAD_PRIORITY 30
#define CYGNUM_JFFS2_GC_THREAD_PRIORITY_30
#define CYGNUM_JFFS2_GC_THREAD_STACK_SIZE 8192
#define CYGNUM_JFFS2_GC_THREAD_STACK_SIZE_8192
#define CYGNUM_JFFS2_GS_THREAD_TICKS 100
#define CYGNUM_JFFS2_GS_THREAD_TICKS_100
#define CYGOPT_FS_JFFS2_DEBUG 0
#define CYGOPT_FS_JFFS2_DEBUG_0
#define CONFIG_JFFS2_FS_DEBUG 0
#define CONFIG_JFFS2_FS_DEBUG_0
#define CYGNUM_FS_JFFS2_RAW_NODE_REF_CACHE_POOL_SIZE 0
#define CYGNUM_FS_JFFS2_RAW_NODE_REF_CACHE_POOL_SIZE_0

struct _inode;
struct super_block;

struct iovec {
        void *iov_base;
        ssize_t iov_len;
};

static inline unsigned int full_name_hash(const unsigned char * name,
		unsigned int len) {

	unsigned hash = 0;
 	while (len--) {
		hash = (hash << 4) | (hash >> 28);
		hash ^= *(name++);
	}
	return hash;
}

/* NAND flash not currently supported on eCos */
#define jffs2_can_mark_obsolete(c) (1)

#define JFFS2_INODE_INFO(i) (&(i)->jffs2_i)
#define OFNI_EDONI_2SFFJ(f)  ((struct _inode *) ( ((char *)f) - ((char *)(&((struct _inode *)NULL)->jffs2_i)) ) )

#define JFFS2_F_I_SIZE(f) (OFNI_EDONI_2SFFJ(f)->i_size)
#define JFFS2_F_I_MODE(f) (OFNI_EDONI_2SFFJ(f)->i_mode)
#define JFFS2_F_I_UID(f) (OFNI_EDONI_2SFFJ(f)->i_uid)
#define JFFS2_F_I_GID(f) (OFNI_EDONI_2SFFJ(f)->i_gid)
#define JFFS2_F_I_CTIME(f) (OFNI_EDONI_2SFFJ(f)->i_ctime)
#define JFFS2_F_I_MTIME(f) (OFNI_EDONI_2SFFJ(f)->i_mtime)
#define JFFS2_F_I_ATIME(f) (OFNI_EDONI_2SFFJ(f)->i_atime)

/* FIXME: embox doesn't hav a concept of device major/minor numbers */
#define JFFS2_F_I_RDEV_MIN(f) ((OFNI_EDONI_2SFFJ(f)->i_rdev)&0xff)
#define JFFS2_F_I_RDEV_MAJ(f) ((OFNI_EDONI_2SFFJ(f)->i_rdev)>>8)

struct _inode {
	uint32_t		i_ino;

	int			i_count;
	mode_t			i_mode;
	nlink_t			i_nlink; /* Could we dispense with this? */
	uid_t			i_uid;
	gid_t			i_gid;
	time_t			i_atime;
	time_t			i_mtime;
	time_t			i_ctime;

	unsigned short	i_rdev; /* For devices only */
	struct _inode *	i_parent; /* For directories only */
	off_t		i_size; /* For files only */

	struct super_block *	i_sb;

	struct jffs2_inode_info	jffs2_i;

	struct _inode *		i_cache_prev; /* We need doubly-linked? */
	struct _inode *		i_cache_next;
};

struct super_block {
	struct jffs2_sb_info jffs2_sb;
	struct _inode *s_root;
    unsigned long  s_mount_count;
	struct block_dev *bdev;

#ifdef CYGOPT_FS_JFFS2_GCTHREAD
	struct mutex s_lock;        /* Lock the inode cache */
	struct thread s_gc_thread;
    char s_gc_thread_stack[CYGNUM_JFFS2_GC_THREAD_STACK_SIZE];
//       mtab_entry *mte;
#endif
};

#define sleep_on_spinunlock(wq, sl) spin_unlock(sl)
#define EBADFD 32767

/* background.c */
#ifdef CYGOPT_FS_JFFS2_GCTHREAD
void jffs2_garbage_collect_trigger(struct jffs2_sb_info *c);
void jffs2_start_garbage_collect_thread(struct jffs2_sb_info *c);
void jffs2_stop_garbage_collect_thread(struct jffs2_sb_info *c);
#else
static inline void jffs2_garbage_collect_trigger(struct jffs2_sb_info *c) {
	/* We don't have a GC thread */
}
#endif

struct _inode *jffs2_new_inode (struct _inode *dir_i,
					int mode, struct jffs2_raw_inode *ri);
struct _inode *jffs2_iget(struct super_block *sb, uint32_t ino);
void jffs2_iput(struct _inode * i);
void jffs2_gc_release_inode(struct jffs2_sb_info *c,
							struct jffs2_inode_info *f);
struct jffs2_inode_info *jffs2_gc_fetch_inode(struct jffs2_sb_info *c,
													int inum, int nlink);
unsigned char *jffs2_gc_fetch_page(struct jffs2_sb_info *c,
		struct jffs2_inode_info *f, unsigned long offset, unsigned long *priv);
void jffs2_gc_release_page(struct jffs2_sb_info *c,
				unsigned char *pg, unsigned long *priv);

/* Avoid polluting eCos namespace with names not starting in jffs2_ */
#define os_to_jffs2_mode(x) jffs2_from_os_mode(x)
uint32_t jffs2_from_os_mode(uint32_t osmode);
uint32_t jffs2_to_os_mode (uint32_t jmode);


/* flashio.c */
bool jffs2_flash_read(struct jffs2_sb_info *c,
		uint32_t read_buffer_offset, const size_t size,
		size_t * return_size, unsigned char * write_buffer);
bool jffs2_flash_write(struct jffs2_sb_info *c, uint32_t write_buffer_offset,
			   const size_t size, size_t * return_size,
			   unsigned char * read_buffer);
int jffs2_flash_direct_writev(struct jffs2_sb_info *c,
		const struct iovec *vecs, unsigned long count,
		loff_t to, size_t *retlen);
bool jffs2_flash_erase(struct jffs2_sb_info *c, struct jffs2_eraseblock *jeb);

/* dir.c */
struct _inode *jffs2_lookup(struct _inode *dir_i,
		const unsigned char *name, int namelen);
int jffs2_create(struct _inode *dir_i, const unsigned char *d_name,
		int mode, struct _inode **new_i);
int jffs2_mkdir (struct _inode *dir_i, const unsigned char *d_name, int mode);
int jffs2_link (struct _inode *old_d_inode, struct _inode *dir_i,
		const unsigned char *d_name);
int jffs2_unlink(struct _inode *dir_i, struct _inode *d_inode,
		const unsigned char *d_name);
int jffs2_rmdir (struct _inode *dir_i, struct _inode *d_inode,
		const unsigned char *d_name);
int jffs2_rename (struct _inode *old_dir_i, struct _inode *d_inode,
		const unsigned char *old_d_name,
		  struct _inode *new_dir_i, const unsigned char *new_d_name);

/* erase.c */
static inline void jffs2_erase_pending_trigger(struct jffs2_sb_info *c) { }

#ifndef CONFIG_JFFS2_FS_WRITEBUFFER
#define SECTOR_ADDR(x) ( ((unsigned long)(x) & ~(c->sector_size-1)) )
#define jffs2_can_mark_obsolete(c) (1)
#define jffs2_cleanmarker_oob(c) (0)
#define jffs2_write_nand_cleanmarker(c,jeb) (-EIO)

#define jffs2_flush_wbuf_pad(c) (c=c)
#define jffs2_flush_wbuf_gc(c, i) ({ (void)(c), (void) i, 0; })
#define jffs2_nand_read_failcnt(c,jeb) do { ; } while(0)
#define jffs2_write_nand_badblock(c,jeb,p) (0)
#define jffs2_flash_setup(c) (0)
#define jffs2_nand_flash_cleanup(c) do {} while(0)
#define jffs2_wbuf_dirty(c) (0)
#define jffs2_flash_writev(a,b,c,d,e,f) jffs2_flash_direct_writev(a,b,c,d,e)
#define jffs2_wbuf_timeout NULL
#define jffs2_wbuf_process NULL
#define jffs2_nor_ecc(c) (0)
#else
#error no nand yet
#endif

#ifndef BUG_ON
#define BUG_ON(x) do { if (unlikely(x)) BUG(); } while(0)
#endif

#define __init

typedef struct jffs2_fs_info {
	char mntto[PATH_MAX];
	struct super_block jffs2_sb;
} jffs2_fs_info_t;

typedef struct jffs2_file_info {
	struct _inode *_inode;
} jffs2_file_info_t;

#endif /* __JFFS2_H__ */
