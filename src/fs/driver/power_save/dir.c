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

#include <string.h>
#include <linux/kernel.h>
#include <kernel/printk.h>

#include <fs/file_operation.h>

#include "qnx6.h"

static unsigned qnx6_lfile_checksum(char *name, unsigned size)
{
	unsigned crc = 0;
	char *end = name + size;
	while (name < end) {
		crc = ((crc >> 1) + *(name++)) ^
			((crc & 0x00000001) ? 0x80000000 : 0);
	}
	return crc;
}

static struct buffer_head *qnx6_get_bh(struct node *dir, unsigned long n)
{
	struct buffer_head *bh;
	bh = qnx6_get_block(dir, n, 0);

	return (struct buffer_head *) bh;
}

static inline unsigned long dir_pages(struct node *node)
{
	return (QNX6_I(node)->i_size+QNX6_BH_BLOCK_SIZE-1)>>QNX6_BH_BLOCK_SHIFT;
}

static unsigned last_entry(struct node *node, unsigned long page_nr)
{
	unsigned long last_byte = QNX6_I(node)->i_size;
	last_byte -= page_nr << QNX6_BH_BLOCK_SHIFT;
	if (last_byte > QNX6_BH_BLOCK_SIZE)
		last_byte = QNX6_BH_BLOCK_SIZE;
	return last_byte / QNX6_DIR_ENTRY_SIZE;
}

static struct qnx6_long_filename *qnx6_longname(struct qnx6_superblock *sb,
					 struct qnx6_long_dir_entry *de,
					 struct buffer_head **p)
{
	struct buffer_head *bh;
	struct qnx6_sb_info *sbi = QNX6_SB(sb);
	__u32 s = fs32_to_cpu(sbi, de->de_long_node); /* in block units */
	__u32 n = s >> (QNX6_BH_BLOCK_SHIFT - sb->s_blocksize_bits); /* in pages */
	__u32 offs = (s << sb->s_blocksize_bits) & ~QNX6_BH_BLOCK_MASK;

	bh = qnx6_get_block(sbi->longfile, n, 0);
	if (!bh)
		return NULL;

	return (struct qnx6_long_filename *)(bh->data + offs);

}

static int qnx6_dir_longfilename(struct node *node,
			struct qnx6_long_dir_entry *de,
			void *dirent, loff_t pos,
			unsigned de_node, filldir_t filldir)
{
	struct qnx6_long_filename *lf;
	struct qnx6_superblock *s = QNX6_I(node)->i_sb;
	struct qnx6_sb_info *sbi = QNX6_SB(s);
	struct buffer_head *buffer_head;
	int lf_size;

	if (de->de_size != 0xff) {
		/* error - long filename entries always have size 0xff
		   in direntry */
		printk(KERN_ERR "qnx6: invalid direntry size (%i).\n",
				de->de_size);
		return 0;
	}
	lf = qnx6_longname(s, de, &buffer_head);
	if (IS_ERR(lf)) {
		printk(KERN_ERR "qnx6:Error reading longname\n");
		return 0;
	}

	lf_size = fs16_to_cpu(sbi, lf->lf_size);

	if (lf_size > QNX6_LONG_NAME_MAX) {
		QNX6DEBUG((KERN_INFO "file %s\n", lf->lf_fname));
		printk(KERN_ERR "qnx6:Filename too long (%i)\n", lf_size);
		qnx6_embox_put_bh(buffer_head);
		return 0;
	}

	/* calc & validate longfilename checksum
	   mmi 3g filesystem does not have that checksum */
	if (!test_opt(s, MMI_FS) && fs32_to_cpu(sbi, de->de_checksum) !=
			qnx6_lfile_checksum((char *) lf->lf_fname, lf_size))
		printk(KERN_INFO "qnx6: long filename checksum error.\n");

	QNX6DEBUG((KERN_INFO "qnx6_readdir:%.*s node:%u\n",
					lf_size, lf->lf_fname, de_node));
	if (filldir(dirent, (char *) lf->lf_fname, lf_size, pos, de_node,
			DT_UNKNOWN) < 0) {
		qnx6_embox_put_bh(buffer_head);
		return 0;
	}

	qnx6_embox_put_bh(buffer_head);
	/* success */
	return 1;
}

int qnx6_readdir(struct qnx_file *filp, void *dirent, filldir_t filldir)
{
	struct node *node = qnx6_file_node(filp);
	struct qnx6_node_info *ei = QNX6_I(node);
	struct qnx6_superblock *s = ei->i_sb;
	struct qnx6_sb_info *sbi = QNX6_SB(s);
	loff_t pos = filp->f_pos & ~(QNX6_DIR_ENTRY_SIZE - 1);
	unsigned long npages = dir_pages(node);
	unsigned long n = pos >> QNX6_BH_BLOCK_SHIFT;
	unsigned start = (pos & ~QNX6_BH_BLOCK_MASK) / QNX6_DIR_ENTRY_SIZE;
	bool done = false;

	if (filp->f_pos >= ei->i_size)
		return 0;

	for ( ; !done && n < npages; n++, start = 0) {
		struct buffer_head *buffer_head = qnx6_get_bh(node, n);
		int limit = last_entry(node, n);
		struct qnx6_dir_entry *de;
		int i = start;

		if (IS_ERR(buffer_head)) {
			printk(KERN_ERR "qnx6_readdir: read failed\n");
			filp->f_pos = (n + 1) << QNX6_BH_BLOCK_SHIFT;
			return PTR_ERR(buffer_head);
		}
		de = ((struct qnx6_dir_entry *)embox_bh_data(buffer_head)) + start; //XXX
		for (; i < limit; i++, de++, pos += QNX6_DIR_ENTRY_SIZE) {
			int size = de->de_size;
			__u32 no_node = fs32_to_cpu(sbi, de->de_node);

			if (!no_node || !size)
				continue;

			if (size > QNX6_SHORT_NAME_MAX) {
				/* long filename detected
				   get the filename from long filename
				   structure / block */
				if (!qnx6_dir_longfilename(node,
					(struct qnx6_long_dir_entry *)de,
					dirent, pos, no_node,
					filldir)) {
					done = true;
					break;
				}
			} else {
				QNX6DEBUG((KERN_INFO "qnx6_readdir:%.*s"
				   " node:%u\n", size, de->de_fname,
							no_node));
				if (filldir(dirent, de->de_fname, size,
				      pos, no_node, DT_UNKNOWN)
					< 0) {
					done = true;
					break;
				}
			}
		}
		qnx6_embox_put_bh(buffer_head);
	}
	filp->f_pos = pos;
	return 0;
}

#if 0
/*
 * check if the long filename is correct.
 */
static unsigned qnx6_long_match(int len, const char *name,
			struct qnx6_long_dir_entry *de, struct node *dir)
{
	struct qnx6_superblock *s = QNX6_I(dir)->i_sb;
	struct qnx6_sb_info *sbi = QNX6_SB(s);
	struct page *page;
	int thislen;
	struct qnx6_long_filename *lf = qnx6_longname(s, de, &page);

	if (IS_ERR(lf))
		return 0;

	thislen = fs16_to_cpu(sbi, lf->lf_size);
	if (len != thislen) {
		qnx6_embox_put_page(page);
		return 0;
	}
	if (memcmp(name, lf->lf_fname, len) == 0) {
		qnx6_embox_put_page(page);
		return fs32_to_cpu(sbi, de->de_node);
	}
	qnx6_embox_put_page(page);
	return 0;
}

/*
 * check if the filename is correct.
 */
static unsigned qnx6_match(struct qnx6_superblock *s, int len, const char *name,
			struct qnx6_dir_entry *de)
{
	struct qnx6_sb_info *sbi = QNX6_SB(s);
	if (memcmp(name, de->de_fname, len) == 0)
		return fs32_to_cpu(sbi, de->de_node);
	return 0;
}


static unsigned qnx6_find_entry(int len, struct node *dir, const char *name,
			 struct page **res_page)
{
	struct qnx6_superblock *s = QNX6_I(dir)->i_sb;
	struct qnx6_node_info *ei = QNX6_I(dir);
	struct page *page = NULL;
	unsigned long start, n;
	unsigned long npages = dir_pages(dir);
	unsigned ino;
	struct qnx6_dir_entry *de;
	struct qnx6_long_dir_entry *lde;

	*res_page = NULL;

	if (npages == 0)
		return 0;
	start = ei->i_dir_start_lookup;
	if (start >= npages)
		start = 0;
	n = start;

	do {
		page = qnx6_get_page(dir, n);
		if (!IS_ERR(page)) {
			int limit = last_entry(dir, n);
			int i;

			de = (struct qnx6_dir_entry *)embox_page_address(page);
			for (i = 0; i < limit; i++, de++) {
				if (len <= QNX6_SHORT_NAME_MAX) {
					/* short filename */
					if (len != de->de_size)
						continue;
					ino = qnx6_match(s, len, name, de);
					if (ino)
						goto found;
				} else if (de->de_size == 0xff) {
					/* deal with long filename */
					lde = (struct qnx6_long_dir_entry *)de;
					ino = qnx6_long_match(len,
								name, lde, dir);
					if (ino)
						goto found;
				} else
					printk(KERN_ERR "qnx6: undefined "
						"filename size in node.\n");
			}
			qnx6_embox_put_page(page);
		}

		if (++n >= npages)
			n = 0;
	} while (n != start);
	return 0;

found:
	*res_page = page;
	ei->i_dir_start_lookup = n;
	return ino;
}
#endif
