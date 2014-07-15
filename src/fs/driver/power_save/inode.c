/*
 * QNX6 file system, Linux implementation.
 *
 * Version : 1.0.0
 *
 * History :
 *
 * 01-02-2012 by Kai Bankett (chaosman@ontika.net) : first release.
 * 16-02-2012 pagemap extension by Al Viro
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

#include <fs/bcache.h>

#define b_data data

#include <fs/vfs.h>
#include <fs/hlpr_path.h>
#include <fs/node.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <kernel/printk.h>
#include <linux/kernel.h>
#include <linux/crc32.h>
#include <mem/misc/pool.h>

#include <fs/fs_driver.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>

#include <util/bit.h>

#include "qnx6.h"

static int qnx6fs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int qnx6fs_close(struct file_desc *desc);
static size_t qnx6fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t qnx6fs_write(struct file_desc *desc, void *buf, size_t size);
static int qnx6fs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations qnx6_fop = {
	.open = qnx6fs_open,
	.close = qnx6fs_close,
	.read = qnx6fs_read,
	.write = qnx6fs_write,
	.ioctl = qnx6fs_ioctl,
};


struct qnx6_fs_info {
	struct qnx6_superblock f_sb;
};

POOL_DEF(qnx6_fsi_pool, struct qnx6_fs_info, OPTION_GET(NUMBER, fs_info_count));
POOL_DEF(qnx6_node_pool, struct qnx6_node_info, OPTION_GET(NUMBER, node_info_count));

typedef unsigned long sector_t;

uint32_t crc32_be(uint32_t crc, unsigned char const *p, size_t len) {
	uint32_t v = crc32_accumulate(crc, (unsigned char *) p, len);

	return v;
}
static void qnx6_put_super(struct qnx6_superblock *sb) __attribute__((used));
static struct node *qnx6_alloc_node(struct qnx6_superblock *sb, const char *name,
	       	size_t namelen);
static void qnx6_destroy_node(struct node *node) __attribute__((used));
static int qnx6_statfs(struct dentry *dentry, void *buf) __attribute__((used));

static unsigned qnx6_get_devblock(struct qnx6_superblock *sb, __fs32 block)
{
	struct qnx6_sb_info *sbi = QNX6_SB(sb);
	return fs32_to_cpu(sbi, block) + sbi->s_blks_off;
}

static unsigned qnx6_block_map(struct node *node, unsigned iblock);

struct buffer_head *qnx6_get_block(struct node *node, unsigned int iblock,
		int create)
{
	unsigned phys;

	QNX6DEBUG((KERN_INFO "qnx6: qnx6_get_block node=[%ld] iblock=[%ld]\n",
			(long) 0 /*node->i_ino*/, (unsigned long)iblock));

	phys = qnx6_block_map(node, iblock);
	if (phys) {
		/* logical block is before EOF */
		return sb_bread(QNX6_I(node)->i_sb, phys);
	}
	return NULL;
}

static int qnx6_check_blockptr(__fs32 ptr)
{
	if (ptr == ~(__fs32)0) {
		printk(KERN_ERR "qnx6: hit unused blockpointer.\n");
		return 0;
	}
	return 1;
}

/*
 * returns the block number for the no-th element in the tree
 * nodebits requred as there are multiple nodes in one node block
 */
static unsigned qnx6_block_map(struct node *node, unsigned no)
{
	struct qnx6_node_info *ei = QNX6_I(node);
	struct qnx6_superblock *s = ei->i_sb;
	struct qnx6_sb_info *sbi = QNX6_SB(s);
	unsigned block = 0;
	struct buffer_head *bh;
	__fs32 ptr;
	int levelptr;
	int ptrbits = sbi->s_ptrbits;
	int bitdelta;
	__u32 mask = (1 << ptrbits) - 1;
	int depth = ei->di_filelevels;
	int i;

	bitdelta = ptrbits * depth;
	levelptr = no >> bitdelta;

	if (levelptr > QNX6_NO_DIRECT_POINTERS - 1) {
		printk(KERN_ERR "qnx6:Requested file block number (%u) too big.",
				no);
		return 0;
	}

	block = qnx6_get_devblock(s, ei->di_block_ptr[levelptr]);

	for (i = 0; i < depth; i++) {
		bh = sb_bread(s, block);
		if (!bh) {
			printk(KERN_ERR "qnx6:Error reading block (%u)\n",
					block);
			return 0;
		}
		bitdelta -= ptrbits;
		levelptr = (no >> bitdelta) & mask;
		ptr = ((__fs32 *)bh->b_data)[levelptr];

		if (!qnx6_check_blockptr(ptr))
			return 0;

		block = qnx6_get_devblock(s, ptr);
		brelse(bh);
	}
	return block;
}

/*static int qnx6_statfs(struct dentry *dentry, struct kstatfs *buf)*/ //XXX
static int qnx6_statfs(struct dentry *dentry, void *buf)
{
#if 0
	/*struct qnx6_superblock *sb = dentry->d_sb;*/ //XXX
	struct qnx6_superblock *sb = NULL;
	struct qnx6_sb_info *sbi = QNX6_SB(sb);
	/*__u64 id = huge_encode_dev(sb->s_bdev->bd_dev);*/ //XXX


	buf->f_type    = sb->s_magic;
	buf->f_bsize   = sb->s_blocksize;
	buf->f_blocks  = fs32_to_cpu(sbi, sbi->sb->sb_num_blocks);
	buf->f_bfree   = fs32_to_cpu(sbi, sbi->sb->sb_free_blocks);
	buf->f_files   = fs32_to_cpu(sbi, sbi->sb->sb_num_nodes);
	buf->f_ffree   = fs32_to_cpu(sbi, sbi->sb->sb_free_nodes);
	buf->f_bavail  = buf->f_bfree;
	buf->f_namelen = QNX6_LONG_NAME_MAX;
	buf->f_fsid.val[0] = (u32)id;
	buf->f_fsid.val[1] = (u32)(id >> 32);
#endif

	return 0;
}

/*
 * Check the root directory of the filesystem to make sure
 * it really _is_ a qnx6 filesystem, and to check the size
 * of the directory entry.
 */
static const char *qnx6_checkroot(struct qnx6_superblock *s)
{
#if 0
	static char match_root[2][3] = {".\0\0", "..\0"};
	int i, error = 0;
	struct qnx6_dir_entry *dir_entry;
	struct node *root = s->s_root->d_node;
	struct address_space *mapping = root->i_mapping;
	struct page *page = read_mapping_page(mapping, 0, NULL);
	if (IS_ERR(page))
		return "error reading root directory";
	kmap(page);
	dir_entry = page_address(page);
	for (i = 0; i < 2; i++) {
		/* maximum 3 bytes - due to match_root limitation */
		if (strncmp(dir_entry[i].de_fname, match_root[i], 3))
			error = 1;
	}
	qnx6_put_page(page);
	if (error)
		return "error reading root directory.";
#endif
	return NULL;
}

#ifdef CONFIG_QNX6FS_DEBUG
void qnx6_superblock_debug(struct qnx6_super_block *sb, struct qnx6_superblock *s)
{
	struct qnx6_sb_info *sbi = QNX6_SB(s);

	QNX6DEBUG((KERN_INFO "magic: %08x\n",
				fs32_to_cpu(sbi, sb->sb_magic)));
	QNX6DEBUG((KERN_INFO "checksum: %08x\n",
				fs32_to_cpu(sbi, sb->sb_checksum)));
	QNX6DEBUG((KERN_INFO "serial: %llx\n",
				fs64_to_cpu(sbi, sb->sb_serial)));
	QNX6DEBUG((KERN_INFO "flags: %08x\n",
				fs32_to_cpu(sbi, sb->sb_flags)));
	QNX6DEBUG((KERN_INFO "blocksize: %08x\n",
				fs32_to_cpu(sbi, sb->sb_blocksize)));
	QNX6DEBUG((KERN_INFO "num_nodes: %08x\n",
				fs32_to_cpu(sbi, sb->sb_num_nodes)));
	QNX6DEBUG((KERN_INFO "free_nodes: %08x\n",
				fs32_to_cpu(sbi, sb->sb_free_nodes)));
	QNX6DEBUG((KERN_INFO "num_blocks: %08x\n",
				fs32_to_cpu(sbi, sb->sb_num_blocks)));
	QNX6DEBUG((KERN_INFO "free_blocks: %08x\n",
				fs32_to_cpu(sbi, sb->sb_free_blocks)));
	QNX6DEBUG((KERN_INFO "node_levels: %02x\n",
				sb->Inode.levels));
}
#endif

static struct buffer_head *qnx6_check_first_superblock(struct qnx6_superblock *s,
				int offset, int silent)
{
	struct qnx6_sb_info *sbi = QNX6_SB(s);
	struct buffer_head *bh;
	struct qnx6_super_block *sb;

	/* Check the superblock signatures
	   start with the first superblock */
	bh = sb_bread(s, offset);
	if (!bh) {
		printk(KERN_ERR "qnx6: unable to read the first superblock\n");
		return NULL;
	}
	sb = (struct qnx6_super_block *)bh->b_data;
	if (fs32_to_cpu(sbi, sb->sb_magic) != QNX6_SUPER_MAGIC) {
		sbi->s_bytesex = BYTESEX_BE;
		if (fs32_to_cpu(sbi, sb->sb_magic) == QNX6_SUPER_MAGIC) {
			/* we got a big endian fs */
			QNX6DEBUG((KERN_INFO "qnx6: fs got different"
					" endianness.\n"));
			return bh;
		} else
			sbi->s_bytesex = BYTESEX_LE;
		if (!silent) {
			if (offset == 0) {
				printk(KERN_ERR "qnx6: wrong signature (magic)"
					" in superblock #1.\n");
			} else {
				printk(KERN_INFO "qnx6: wrong signature (magic)"
					" at position (0x%lx) - will try"
					" alternative position (0x0000).\n",
						offset * s->s_blocksize);
			}
		}
		brelse(bh);
		return NULL;
	}
	return bh;
}

static void iput(struct node *node) {
	qnx6_destroy_node(node);
}

static struct node *qnx6_private_node(struct qnx6_superblock *s,
					struct qnx6_root_node *p);

static struct qnx6_sb_info __alloc_sbi;
static char __alloc_alloced;
static struct qnx6_sb_info *qnx6_sb_info_alloc(void) {

	assert(__alloc_alloced == 0);

	__alloc_alloced = 1;

	return &__alloc_sbi;
}

static void qnx6_sb_info_free(struct qnx6_sb_info *sb) {
	__alloc_alloced = 0;
}

static int qnx6_fill_super(struct qnx6_superblock *s, void *data, int silent) __attribute__((used));
static int qnx6_fill_super(struct qnx6_superblock *s, void *data, int silent)
{
	struct buffer_head *bh1 = NULL, *bh2 = NULL;
	struct qnx6_super_block *sb1 = NULL, *sb2 = NULL;
	struct qnx6_sb_info *sbi;
	struct node *root;
	const char *errmsg;
	struct qnx6_sb_info *qs;
	int ret = -EINVAL;
	__u64 offset;
	int bootblock_offset = QNX6_BOOTBLOCK_SIZE;

	qs = qnx6_sb_info_alloc();
	if (!qs)
		return -ENOMEM;
	s->s_fs_info = qs;

	/* Superblock always is 512 Byte long */
	if (!qnx6_sb_set_blocksize(s, QNX6_SUPERBLOCK_SIZE)) {
		printk(KERN_ERR "qnx6: unable to set blocksize\n");
		goto outnobh;
	}

#if 0
	/* parse the mount-options */
	if (test_opt(s, MMI_FS)) {
		sb1 = qnx6_mmi_fill_super(s, silent);
		if (sb1)
			goto mmi_success;
		else
			goto outnobh;
	}
#endif

	sbi = QNX6_SB(s);
	sbi->s_bytesex = BYTESEX_LE;
	/* Check the superblock signatures
	   start with the first superblock */
	bh1 = qnx6_check_first_superblock(s,
		bootblock_offset / QNX6_SUPERBLOCK_SIZE, silent);
	if (!bh1) {
		/* try again without bootblock offset */
		bh1 = qnx6_check_first_superblock(s, 0, silent);
		if (!bh1) {
			printk(KERN_ERR "qnx6: unable to read the first superblock\n");
			goto outnobh;
		}
		/* seems that no bootblock at partition start */
		bootblock_offset = 0;
	}
	sb1 = (struct qnx6_super_block *)bh1->b_data;

#ifdef CONFIG_QNX6FS_DEBUG
	qnx6_superblock_debug(sb1, s);
#endif

	/* checksum check - start at byte 8 and end at byte 512 */
#if 0
	if (fs32_to_cpu(sbi, sb1->sb_checksum) !=
			crc32_be(0, (unsigned char *)(bh1->b_data + 8), 504)) {
		printk(KERN_ERR "qnx6: superblock #1 checksum error\n");
		goto out;
	}
#endif

	/* set new blocksize */
	if (!qnx6_sb_set_blocksize(s, fs32_to_cpu(sbi, sb1->sb_blocksize))) {
		printk(KERN_ERR "qnx6: unable to set blocksize\n");
		goto out;
	}
	/* blocksize invalidates bh - pull it back in */
	brelse(bh1);
	bh1 = sb_bread(s, bootblock_offset >> s->s_blocksize_bits);
	if (!bh1)
		goto outnobh;
	sb1 = (struct qnx6_super_block *)bh1->b_data;

	/* calculate second superblock blocknumber */
	offset = fs32_to_cpu(sbi, sb1->sb_num_blocks) +
		(bootblock_offset >> s->s_blocksize_bits) +
		(QNX6_SUPERBLOCK_AREA >> s->s_blocksize_bits);

	/* set bootblock offset */
	sbi->s_blks_off = (bootblock_offset >> s->s_blocksize_bits) +
			  (QNX6_SUPERBLOCK_AREA >> s->s_blocksize_bits);

	/* next the second superblock */
	bh2 = sb_bread(s, offset);
	if (!bh2) {
		printk(KERN_ERR "qnx6: unable to read the second superblock\n");
		goto out;
	}
	sb2 = (struct qnx6_super_block *)bh2->b_data;
	if (fs32_to_cpu(sbi, sb2->sb_magic) != QNX6_SUPER_MAGIC) {
		if (!silent)
			printk(KERN_ERR "qnx6: wrong signature (magic)"
					" in superblock #2.\n");
		goto out;
	}

	/* checksum check - start at byte 8 and end at byte 512 */
#if 0
	if (fs32_to_cpu(sbi, sb2->sb_checksum) !=
				crc32_be(0, (unsigned char *)(bh2->b_data + 8), 504)) {
		printk(KERN_ERR "qnx6: superblock #2 checksum error\n");
		goto out;
	}
#endif

	if (fs64_to_cpu(sbi, sb1->sb_serial) >=
					fs64_to_cpu(sbi, sb2->sb_serial)) {
		/* superblock #1 active */
		sbi->sb_buf = bh1;
		sbi->sb = (struct qnx6_super_block *)bh1->b_data;
		brelse(bh2);
		printk(KERN_INFO "qnx6: superblock #1 active\n");
	} else {
		/* superblock #2 active */
		sbi->sb_buf = bh2;
		sbi->sb = (struct qnx6_super_block *)bh2->b_data;
		brelse(bh1);
		printk(KERN_INFO "qnx6: superblock #2 active\n");
	}
/*mmi_success:*/
	/* sanity check - limit maximum indirect pointer levels */
	if (sb1->Inode.levels > QNX6_PTR_MAX_LEVELS) {
		printk(KERN_ERR "qnx6: too many node levels (max %i, sb %i)\n",
			QNX6_PTR_MAX_LEVELS, sb1->Inode.levels);
		goto out;
	}
	if (sb1->Longfile.levels > QNX6_PTR_MAX_LEVELS) {
		printk(KERN_ERR "qnx6: too many longfilename levels"
				" (max %i, sb %i)\n",
			QNX6_PTR_MAX_LEVELS, sb1->Longfile.levels);
		goto out;
	}

	/* ease the later tree level calculations */
	sbi = QNX6_SB(s);
	sbi->s_ptrbits = bit_ctz(s->s_blocksize / 4);
	sbi->nodes = qnx6_private_node(s, &sb1->Inode);
	if (!sbi->nodes)
		goto out;
	sbi->longfile = qnx6_private_node(s, &sb1->Longfile);
	if (!sbi->longfile)
		goto out1;

	/* prefetch root node */
	root = qnx6_iget(s, QNX6_ROOT_INO);
	if (IS_ERR(root)) {
		printk(KERN_ERR "qnx6: get node failed\n");
		ret = PTR_ERR(root);
		goto out2;
	}

	ret = -ENOMEM;
	s->s_root = root;
#if 0
	if (!s->s_root)
		goto out2;
#endif

	ret = -EINVAL;
	errmsg = qnx6_checkroot(s);
	if (errmsg != NULL) {
		if (!silent)
			printk(KERN_ERR "qnx6: %s\n", errmsg);
		goto out3;
	}
	return 0;

out3:
#if 0
	dput(s->s_root);
	s->s_root = NULL;
#endif
out2:
	iput(sbi->longfile);
out1:
	iput(sbi->nodes);
out:
	if (bh1)
		brelse(bh1);
	if (bh2)
		brelse(bh2);
outnobh:
	qnx6_sb_info_free(qs);
	s->s_fs_info = NULL;
	return ret;
}

static void qnx6_put_super(struct qnx6_superblock *sb)
{
	struct qnx6_sb_info *qs = QNX6_SB(sb);
	brelse(qs->sb_buf);
	iput(qs->longfile);
	iput(qs->nodes);
	qnx6_sb_info_free(qs);
	sb->s_fs_info = NULL;
	return;
}

static struct node *new_node(struct qnx6_superblock *sb) {
	return qnx6_alloc_node(sb, NULL, 0);
}

static struct node *qnx6_private_node(struct qnx6_superblock *s,
					struct qnx6_root_node *p)
{
	struct node *node = new_node(s);
	if (node) {
		struct qnx6_node_info *ei = QNX6_I(node);
		struct qnx6_sb_info *sbi = QNX6_SB(s);
		ei->i_size = fs64_to_cpu(sbi, p->size);
		memcpy(ei->di_block_ptr, p->ptr, sizeof(p->ptr));
		ei->di_filelevels = p->levels;
		node->mode = S_IFREG | S_IRUSR; /* probably wrong */
	}
	return node;
}

static void iget_failed(struct node *node) {
	qnx6_destroy_node(node);
}

#if 0
static struct node *qnx6_embox_iget(struct qnx6_superblock *sb, unsigned ino) {
	return qnx6_alloc_node(sb, NULL, 0);
}
#endif
struct node *qnx6_iget_wname(struct qnx6_superblock *sb, unsigned ino,
		const char *name, size_t namelen)
{
	struct node *node;
	struct qnx6_sb_info *sbi = QNX6_SB(sb);
	struct qnx6_inode_entry *raw_node;
	struct qnx6_node_info	*ei;

	struct buffer_head *bh = NULL;
	__u32 n, offs;

	/*node = iget_locked(sb, ino); */
	/*node = qnx6_embox_iget(sb, ino); */
	node = qnx6_alloc_node(sb, name, namelen);
	if (!node)
		return ERR_PTR(-ENOMEM);
#if 0
	if (!(node->i_state & I_NEW))
		return node;
#endif

	ei = QNX6_I(node);

	node->mode = 0;

	if (ino == 0) {
		printk(KERN_ERR "qnx6: bad node number on dev %s: %u is "
				"out of range\n",
		       /*sb->s_id, ino);*/ //XXX
		       "qnx test", ino);
		iget_failed(node);
		return ERR_PTR(-EIO);
	}
	n = (ino - 1) >> (QNX6_BH_BLOCK_SHIFT - QNX6_INODE_SIZE_BITS);
	offs = (ino - 1) & (~QNX6_BH_BLOCK_MASK >> QNX6_INODE_SIZE_BITS);

	bh = qnx6_get_block(sbi->nodes, n, 0);
	if (!bh) {
		return NULL;
	}

	raw_node = ((struct qnx6_inode_entry *)bh->b_data) + offs;

	node->mode    = fs16_to_cpu(sbi, raw_node->di_mode);
	node->uid = (uid_t)fs32_to_cpu(sbi, raw_node->di_uid);
	node->gid = (gid_t)fs32_to_cpu(sbi, raw_node->di_gid);
	QNX6_I(node)->i_size = fs64_to_cpu(sbi, raw_node->di_size);

#if 0
	node->i_mtime.tv_sec   = fs32_to_cpu(sbi, raw_node->di_mtime);
	node->i_mtime.tv_nsec = 0;
	node->i_atime.tv_sec   = fs32_to_cpu(sbi, raw_node->di_atime);
	node->i_atime.tv_nsec = 0;
	node->i_ctime.tv_sec   = fs32_to_cpu(sbi, raw_node->di_ctime);
	node->i_ctime.tv_nsec = 0;

	/* calc blocks based on 512 byte blocksize */
	node->i_blocks = (node->i_size + 511) >> 9;
#endif

	memcpy(&ei->di_block_ptr, &raw_node->di_block_ptr,
				sizeof(raw_node->di_block_ptr));
	ei->di_filelevels = raw_node->di_filelevels;

	brelse(bh);
	return node;
}

struct node *qnx6_iget(struct qnx6_superblock *sb, unsigned ino) {
	return qnx6_iget_wname(sb, ino, NULL, 0);
}

static struct node *qnx6_alloc_node(struct qnx6_superblock *sb, const char *name,
	       	size_t namelen)
{
	struct qnx6_node_info *ei;
	struct node *node;

	ei = pool_alloc(&qnx6_node_pool);
	if (!ei)
		return NULL;

	node = node_alloc(name, namelen);
	if (!node) {
		pool_free(&qnx6_node_pool, ei);
		return NULL;
	}

	node->nas->fi->privdata = ei;
	ei->i_sb = sb;

	return node;
}

static void qnx6_destroy_node(struct node *node)
{
	pool_free(&qnx6_node_pool, QNX6_I(node));
	node_free(node);
}
/*
 * file_operation
 */
static int qnx6fs_open(struct node *node, struct file_desc *desc, int flags) {

	return 0;
}

static int qnx6fs_close(struct file_desc *desc) {
	return 0;
}

static size_t qnx6fs_read(struct file_desc *desc, void *buff, size_t size) {
	struct buffer_head *bh;
	unsigned iblk;
	off_t offs;
	void *obuf = buff;

	while (size > 0) {
		int toread;

		iblk = desc->cursor >> QNX6_BH_BLOCK_SHIFT;
		offs = desc->cursor & (~QNX6_BH_BLOCK_MASK);

		toread = min(size, min(QNX6_BH_BLOCK_SIZE - offs,
					QNX6_I(desc->node)->i_size - desc->cursor));
		if (toread <= 0) {
			break;
		}

		bh = qnx6_get_block(desc->node, iblk, 0);
		if (!bh) {
			return -ENOMEM;
		}

		memcpy(buff, bh->data + offs, toread);

		brelse(bh);

		desc->cursor += toread;
		buff += toread;
		size -= toread;
	}

	return buff - obuf;

}

static size_t qnx6fs_write(struct file_desc *desc, void *buff, size_t size) {

	return -1;
}

static int qnx6fs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int qnx6fs_init(void * par);
static int qnx6fs_format(void *path);
static int qnx6fs_mount(void *dev, void *dir);
static int qnx6fs_create(struct node *parent_node, struct node *node);
static int qnx6fs_delete(struct node *node);
static int qnx6fs_truncate(struct node *node, off_t length);
static int qnx6fs_umount(void *dir);


static struct fsop_desc qnx6_fsop = {
	.init	     = qnx6fs_init,
	.format	     = qnx6fs_format,
	.mount	     = qnx6fs_mount,
	.create_node = qnx6fs_create,
	.delete_node = qnx6fs_delete,

	.getxattr    = NULL,
	.setxattr    = NULL,
	.listxattr   = NULL,

	.truncate    = qnx6fs_truncate,
	.umount      = qnx6fs_umount,
};

static int qnx6fs_init(void * par) {

	return 0;
};

static int qnx6fs_create(struct node *parent_node, struct node *node) {

	return -1;
}

static int qnx6fs_delete(struct node *node) {

	return -1;
}

static int qnx6fs_format(void *dev) {

	return -1;
}

struct dirent_masq {
	struct qnx6_superblock *d_sb;
	struct node *d_parent;
	struct filesystem *d_fs;
};

static int embox_mount_filldir(void *dirent, const char *name, int size,
		loff_t pos, __u64 node, unsigned d_type) {
	struct node *n;
	struct dirent_masq dm;
	struct qnx_file qf;
	struct dirent_masq *dmasq = dirent;

	if (path_is_dotname(name, size)) {
		return 0;
	}

 	n = qnx6_iget_wname(dmasq->d_sb, node, name, size);
	if (IS_ERR(n)) {
		int err = PTR_ERR(n);
		return err;
	}

	n->nas->fs = dmasq->d_fs;
	vfs_add_leaf(n, dmasq->d_parent);

	if (!node_is_directory(n)) {
		return 0;
	}

	qf.f_node = n;
	qf.f_pos = 0;

	dm.d_sb = dmasq->d_sb;
	dm.d_parent = n;
	dm.d_fs = dmasq->d_fs;

	qnx6_readdir(&qf, &dm, embox_mount_filldir);

	return 0;
}

static int qnx6fs_mount(void *dev, void *dir) {
	struct qnx6_fs_info *fsi;
	struct qnx_file qf;
	struct dirent_masq dm;
	struct node *dir_node;
	struct nas *dir_nas;
	int res;

	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dir_nas->fs = filesystem_create("qnx6"))) {
		return -ENOMEM;
	}

	if (NULL == (fsi = pool_alloc(&qnx6_fsi_pool))) {
		filesystem_free(dir_nas->fs);
		return -ENOMEM;
	}

	dir_nas->fs->fsi = fsi;

	/* for me */
	fsi->f_sb.s_bdev = ((struct node *) dev)->nas->fi->privdata;
	/* for `mount' info */
	dir_nas->fs->bdev = fsi->f_sb.s_bdev;

	if ((res = qnx6_fill_super(&fsi->f_sb, NULL, 0))) {
		return res;
	}

	qf.f_node = fsi->f_sb.s_root;
	qf.f_pos = 0;

	dm.d_sb = &fsi->f_sb;
	dm.d_parent = dir;
	dm.d_fs = dir_nas->fs;

	qnx6_readdir(&qf, &dm, embox_mount_filldir);

	return 0;
}

static int qnx6fs_truncate (struct node *node, off_t length) {

	return -1;
}

static int qnx6_umount_entry(struct node *node) {
	struct node *child;

	if(node_is_directory(node)) {
		while(NULL != (child = vfs_subtree_get_child_next(node, NULL))) {
			if(node_is_directory(child)) {
				qnx6_umount_entry(child);
			}

			pool_free(&qnx6_node_pool, QNX6_I(child));
			vfs_del_leaf(child);
		}
	}

	return 0;
}

static int qnx6fs_umount(void *dir) {
	struct node *dir_node;
	struct nas *dir_nas;
	struct qnx6_fs_info *fsi;

	dir_node = dir;
	dir_nas = dir_node->nas;

	/* delete all entry node */
	qnx6_umount_entry(dir_node);

	if(NULL != dir_nas->fs) {
		fsi = dir_nas->fs->fsi;

		qnx6_put_super(&fsi->f_sb);

		pool_free(&qnx6_fsi_pool, fsi);

		filesystem_free(dir_nas->fs);
		dir_nas->fs = NULL;
	}

	return 0;
}

static struct fs_driver qnx6fs_driver = {
	.name = "qnx6",
	.file_op = &qnx6_fop,
	.fsop = &qnx6_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(qnx6fs_driver);
