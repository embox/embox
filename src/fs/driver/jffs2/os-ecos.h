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

#ifndef __JFFS2_OS_ECOS_H__
#define __JFFS2_OS_ECOS_H__

//#include <pkgconf/fs_jffs2.h>
//#include <cyg/io/io.h>
#include <sys/types.h>
//#include <asm/atomic.h>
#include <linux/stat.h>
#include <linux/compiler.h>

//#include <pkgconf/system.h>
//#include <pkgconf/hal.h>
//#include <pkgconf/io_fileio.h>

//#include <cyg/infra/cyg_trac.h>        // tracing macros
//#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

//#include <cyg/fileio/fileio.h>

//#include <cyg/hal/drv_api.h>
//#include <cyg/infra/diag.h>
//
//#include <cyg/io/flash.h>

#include <linux/types.h>
#include <linux/list.h>
#include <asm/bug.h>

#include <kernel/time/clock_source.h>
#define cyg_timestamp ktime_get_timeseconds
#define get_seconds clock_sys_ticks

typedef uint32_t cyg_uint32;

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

#define kvec iovec

static inline unsigned int full_name_hash(const unsigned char * name, unsigned int len) {

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

/* FIXME: eCos doesn't hav a concept of device major/minor numbers */
#define JFFS2_F_I_RDEV_MIN(f) ((OFNI_EDONI_2SFFJ(f)->i_rdev)&0xff)
#define JFFS2_F_I_RDEV_MAJ(f) ((OFNI_EDONI_2SFFJ(f)->i_rdev)>>8)

struct _inode {
	cyg_uint32		i_ino;

	int			i_count;
	mode_t			i_mode;
	nlink_t			i_nlink; // Could we dispense with this?
	uid_t			i_uid;
	gid_t			i_gid;
	time_t			i_atime;
	time_t			i_mtime;
	time_t			i_ctime;
	//	union {
	unsigned short	i_rdev; // For devices only
	struct _inode *	i_parent; // For directories only
	off_t		i_size; // For files only
	//	};
	struct super_block *	i_sb;

	struct jffs2_inode_info	jffs2_i;

	struct _inode *		i_cache_prev; // We need doubly-linked?
	struct _inode *		i_cache_next;
};

//#define JFFS2_SB_INFO(sb) (&(sb)->jffs2_sb)
//#define OFNI_BS_2SFFJ(c)  ((struct super_block *) ( ((char *)c) - ((char *)(&((struct super_block *)NULL)->jffs2_sb)) ) )

struct super_block {
	struct jffs2_sb_info jffs2_sb;
	struct _inode *s_root;
    unsigned long  s_mount_count;
	struct block_dev *bdev;

#ifdef CYGOPT_FS_JFFS2_GCTHREAD
	cyg_mutex_t s_lock;             // Lock the inode cache
	cyg_flag_t  s_gc_thread_flags;  // Communication with the gcthread
	cyg_handle_t s_gc_thread_handle;
	cyg_thread s_gc_thread;
#if (CYGNUM_JFFS2_GC_THREAD_STACK_SIZE >= CYGNUM_HAL_STACK_SIZE_MINIMUM)
        char s_gc_thread_stack[CYGNUM_JFFS2_GC_THREAD_STACK_SIZE];
#else
        char s_gc_thread_stack[CYGNUM_HAL_STACK_SIZE_MINIMUM];
#endif
       cyg_mtab_entry *mte;
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
static inline void jffs2_garbage_collect_trigger(struct jffs2_sb_info *c)
{
	/* We don't have a GC thread in eCos (yet) */
}
#endif

/* fs-ecos.c */
struct _inode *jffs2_new_inode (struct _inode *dir_i, int mode, struct jffs2_raw_inode *ri);
struct _inode *jffs2_iget(struct super_block *sb, cyg_uint32 ino);
void jffs2_iput(struct _inode * i);
void jffs2_gc_release_inode(struct jffs2_sb_info *c, struct jffs2_inode_info *f);
struct jffs2_inode_info *jffs2_gc_fetch_inode(struct jffs2_sb_info *c, int inum, int nlink);
unsigned char *jffs2_gc_fetch_page(struct jffs2_sb_info *c, struct jffs2_inode_info *f,
				   unsigned long offset, unsigned long *priv);
void jffs2_gc_release_page(struct jffs2_sb_info *c, unsigned char *pg, unsigned long *priv);

/* Avoid polluting eCos namespace with names not starting in jffs2_ */
#define os_to_jffs2_mode(x) jffs2_from_os_mode(x)
uint32_t jffs2_from_os_mode(uint32_t osmode);
uint32_t jffs2_to_os_mode (uint32_t jmode);


/* flashio.c */
bool jffs2_flash_read(struct jffs2_sb_info *c, cyg_uint32 read_buffer_offset,
			  const size_t size, size_t * return_size, unsigned char * write_buffer);
bool jffs2_flash_write(struct jffs2_sb_info *c, cyg_uint32 write_buffer_offset,
			   const size_t size, size_t * return_size, unsigned char * read_buffer);
int jffs2_flash_direct_writev(struct jffs2_sb_info *c, const struct iovec *vecs,
			      unsigned long count, loff_t to, size_t *retlen);
bool jffs2_flash_erase(struct jffs2_sb_info *c, struct jffs2_eraseblock *jeb);

// dir-ecos.c
struct _inode *jffs2_lookup(struct _inode *dir_i, const unsigned char *name, int namelen);
int jffs2_create(struct _inode *dir_i, const unsigned char *d_name, int mode, struct _inode **new_i);
int jffs2_mkdir (struct _inode *dir_i, const unsigned char *d_name, int mode);
int jffs2_link (struct _inode *old_d_inode, struct _inode *dir_i, const unsigned char *d_name);
int jffs2_unlink(struct _inode *dir_i, struct _inode *d_inode, const unsigned char *d_name);
int jffs2_rmdir (struct _inode *dir_i, struct _inode *d_inode, const unsigned char *d_name);
int jffs2_rename (struct _inode *old_dir_i, struct _inode *d_inode, const unsigned char *old_d_name,
		  struct _inode *new_dir_i, const unsigned char *new_d_name);

/* erase.c */
static inline void jffs2_erase_pending_trigger(struct jffs2_sb_info *c)
{ }

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
	uint32_t f_pointer;			/* current (BYTE) pointer */
	struct _inode *_inode;
} jffs2_file_info_t;

#endif /* __JFFS2_OS_ECOS_H__ */
