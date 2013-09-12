/*
 * QNX6 file system, Linux implementation.
 *
 * Version : 1.0.0
 *
 * History :
 *
 * 01-02-2012 by Kai Bankett (chaosman@ontika.net) : first release.
 * 16-02-2012 page map extension by Al Viro
 *
 */

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License v2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <stddef.h>
#include <assert.h>

#include <stdbool.h>
#include <linux/fs.h>
#include <fs/bcache.h>

#include <sys/types.h>

#include <fs/node.h>
#include <util/member.h>

typedef __u16 __fs16;
typedef __u32 __fs32;
typedef __u64 __fs64;

#include "qnx6_fs.h"


#if 1
#ifdef CONFIG_QNX6FS_DEBUG
#define QNX6DEBUG(X) printk X
#else
#define QNX6DEBUG(X) (void) 0
#endif
#else
#define QNX6DEBUG(X) printk X
#endif

#define QNX6_BH_BLOCK_SIZE 1024
#define QNX6_BH_BLOCK_SHIFT 10
#define QNX6_BH_BLOCK_MASK  ~(0x3ff) //really?

struct qnx6_superblock {
	unsigned char s_blocksize_bits;
	unsigned long s_blocksize;
	void *s_fs_info;
	block_dev_t *s_bdev;
	struct node *s_root;
};

struct qnx_file {
	struct node *f_node;
	unsigned int f_pos;
};

static inline struct node *qnx6_file_node(struct qnx_file *f) {
	return f->f_node;
}

struct qnx6_sb_info {
	struct buffer_head	*sb_buf;	/* superblock buffer */
	struct qnx6_super_block	*sb;		/* our superblock */
	int			s_blks_off;	/* blkoffset fs-startpoint */
	int			s_ptrbits;	/* indirect pointer bitfield */
	unsigned long		s_mount_opt;	/* all mount options */
	int			s_bytesex;	/* holds endianess info */
	struct node *		nodes;
	struct node *		longfile;
};

struct qnx6_node_info {
	__fs32			di_block_ptr[QNX6_NO_DIRECT_POINTERS];
	__u8			di_filelevels;
	__u32			i_dir_start_lookup;

	/* new added */
	loff_t i_size;
	struct qnx6_superblock *i_sb;

};

typedef int (*filldir_t)(void *, const char *, int, loff_t, __u64, unsigned);

static inline struct buffer_head *sb_bread(struct qnx6_superblock *sb, unsigned int block) {
	struct buffer_head *bh = bcache_getblk_locked(sb->s_bdev, block, sb->s_blocksize);
	unsigned fs_blksize = sb->s_blocksize;
	block_dev_t *bdev = bh->bdev;
	unsigned bdev_blksize;

	/* FIXME */
       	bdev_blksize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);

	if (bh && buffer_new(bh)) {
		/* FIXME */
		if (fs_blksize != bdev->driver->read(bdev, bh->data, fs_blksize,
					block * fs_blksize / bdev_blksize)) {
			bcache_buffer_unlock(bh);
			return NULL;
		}
	}

	return bh;
}

static inline void brelse(struct buffer_head *bh) {
	bcache_buffer_unlock(bh);
}

static inline int qnx6_sb_set_blocksize(struct qnx6_superblock *sb, int size) {

	switch(size) {
	case 512:
		sb->s_blocksize = size;
		sb->s_blocksize_bits = 9;
		break;
	case 1024:
		sb->s_blocksize = size;
		sb->s_blocksize_bits = 10;
		break;
	default:
		assert(0);
	}

	return size;
}

extern struct node *qnx6_iget(struct qnx6_superblock *sb, unsigned ino);
extern struct dentry *qnx6_lookup(struct node *dir, struct dentry *dentry,
					unsigned int flags);

#ifdef CONFIG_QNX6FS_DEBUG
extern void qnx6_superblock_debug(struct qnx6_super_block *,
						struct qnx6_superblock *);
#endif

extern const struct node_operations qnx6_dir_node_operations;
extern const struct file_operations qnx6_dir_operations;

static inline struct qnx6_sb_info *QNX6_SB(struct qnx6_superblock *sb)
{
	return sb->s_fs_info;
}

static inline struct qnx6_node_info *QNX6_I(struct node *node)
{
	return node->nas->fi->privdata;
}

#define clear_opt(o, opt)		(o &= ~(QNX6_MOUNT_##opt))
#define set_opt(o, opt)			(o |= (QNX6_MOUNT_##opt))
#define test_opt(sb, opt)		(QNX6_SB(sb)->s_mount_opt & \
					 QNX6_MOUNT_##opt)
enum {
	BYTESEX_LE,
	BYTESEX_BE,
};

static inline __u64 fs64_to_cpu(struct qnx6_sb_info *sbi, __fs64 n)
{
	//XXX
#if 0
	if (sbi->s_bytesex == BYTESEX_LE)
		return le64_to_cpu(n);
	else
		return be64_to_cpu(n);
#endif
	return n;

}

static inline __fs64 cpu_to_fs64(struct qnx6_sb_info *sbi, __u64 n)
{
	//XXX
#if 0
	if (sbi->s_bytesex == BYTESEX_LE)
		return (__force __fs64)cpu_to_le64(n);
	else
		return (__force __fs64)cpu_to_be64(n);
#endif
	return n;
}

static inline __u32 fs32_to_cpu(struct qnx6_sb_info *sbi, __fs32 n)
{
	//XXX
#if 0
	if (sbi->s_bytesex == BYTESEX_LE)
		return le32_to_cpu((__force __le32)n);
	else
		return be32_to_cpu((__force __be32)n);
#endif
	return n;
}

static inline __fs32 cpu_to_fs32(struct qnx6_sb_info *sbi, __u32 n)
{
	//XXX
#if 0
	if (sbi->s_bytesex == BYTESEX_LE)
		return (__force __fs32)cpu_to_le32(n);
	else
		return (__force __fs32)cpu_to_be32(n);
#endif
	return n;
}

static inline __u16 fs16_to_cpu(struct qnx6_sb_info *sbi, __fs16 n)
{
	//XXX
#if 0
	if (sbi->s_bytesex == BYTESEX_LE)
		return le16_to_cpu((__force __le16)n);
	else
		return be16_to_cpu((__force __be16)n);
#endif
	return n;
}

static inline __fs16 cpu_to_fs16(struct qnx6_sb_info *sbi, __u16 n)
{
	//XXX
#if 0
	if (sbi->s_bytesex == BYTESEX_LE)
		return (__force __fs16)cpu_to_le16(n);
	else
		return (__force __fs16)cpu_to_be16(n);
#endif
	return n;
}

extern struct qnx6_super_block *qnx6_mmi_fill_super(struct qnx6_superblock *s,
						    int silent);

static inline void *embox_bh_data(struct buffer_head *bh) {
	return bh->data;
}

static inline void qnx6_embox_put_bh(struct buffer_head *bh)
{
	brelse(bh);
}

extern int qnx6_readdir(struct qnx_file *filp, void *dirent, filldir_t filldir);

extern struct buffer_head *qnx6_get_block(struct node *node, unsigned int iblock,
			int create);

