/**
 * @file
 * @brief ext2 block allocate
 *
 * @date 25.12.2012
 * @author Andrey Gazukin
 */

/* This files manages blocks allocation and deallocation.
 *
 * The entry points into this file are:
 *   discard_preallocated_blocks:	Discard preallocated blocks.
 *   alloc_block:	somebody wants to allocate a block; find one.
 *   free_block:	indicate that a block is available for new allocation.
 *
 * Created:
 *   June 2010 (Evgeniy Ivanov)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/hlpr_path.h>
#include <lib/libds/array.h>
#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <fs/super_block.h>
#include <fs/file_desc.h>

#include <fs/ext2.h>
#include <fs/ext4.h>

#define DIRTY 1

/* help function */

static int ext4_check_block_number(uint32_t block, struct ext4_fs_info *fsi,
				struct ext4_group_desc *gd)
{
	/* Check if we allocated a data block, but not control (system) block.
	 * Only major bug can cause us to allocate wrong block. If it happens,
	 * we panic (and don't bloat filesystem's bitmap).
	 */
	if (block == ext4_inode_bitmap_len(gd) || block == ext4_block_bitmap_len(gd) ||
		(block >= ext4_inode_table_len(gd)
		&& block < (ext4_inode_table_len(gd) + fsi->s_itb_per_group))) {
		/* panic("ext2: block allocator tryed to return
		 * system/control block, poke author.\n");
		 */
		return 1;
	}

	if (block >= fsi->e4sb.s_blocks_count) {
		/* panic("ext2: allocator returned blocknum greater, than
		 * total number of blocks.\n");
		 */
		return 1;
	}
	return 0;
}

uint32_t ext4_setbit(uint32_t *bitmap, uint32_t max_bits, unsigned int word)
{
	/* Find free bit in bitmap and set. Return number of the bit,
	* if failed return -1.
	*/
	uint32_t *wptr, *wlim;
	uint32_t i, k;
	uint32_t b;

	b = -1;
	/* TODO: do we need to add 1? I saw a situation, when it was
	* required, and since we check bit number with max_bits it
	* should be safe.
	*/
	wlim = &bitmap[FS_BITMAP_CHUNKS(max_bits >> 3)];

	/* Iterate over the words in block. */
	for (wptr = &bitmap[word]; wptr < wlim; wptr++) {
		/* Does this word contain a free bit? */
		if (*wptr == (uint32_t) ~0) {
			continue;
		}

		/* Find and allocate the free bit. */
		k = (int) *wptr;
		for (i = 0; (k & (1 << i)) != 0; ++i) {}

		/* Bit number from the start of the bit map. */
		b = (wptr - &bitmap[0]) * EXT4_FS_BITCHUNK_BITS + i;

		/* Don't allocate bits beyond the end of the map. */
		if (b >= max_bits) {
			b = -1;
			continue;
		}

		/* Allocate bit number. */
		k |= 1 << i;
		*wptr = (int) k;
		break;
	}

	return b;
}

int ext4_unsetbit(uint32_t *bitmap, uint32_t bit)
{
	/* Unset specified bit. If requested bit is already free return -1,
	* otherwise return 0.
	*/
	unsigned int word;		/* bit_returned word in bitmap */
	uint32_t k, mask;

	word = bit / EXT4_FS_BITCHUNK_BITS;
	bit = bit % EXT4_FS_BITCHUNK_BITS; /* index in word */
	mask = 1 << bit;

	k = (int) bitmap[word];
	if (!(k & mask)) {
		return -1;
	}

	k &= ~mask;
	bitmap[word] = (int) k;
	return 0;
}

struct ext4_group_desc *ext4_get_group_desc(unsigned int bnum, struct ext4_fs_info *fsi) {
	if (bnum >= fsi->s_groups_count) {
		return NULL;
	}
	return &fsi->e4fs_gd[bnum];
}

static uint32_t ext4_alloc_block_bit(struct nas *nas, uint32_t goal) { /* try to allocate near this block */
	uint32_t block;	/* allocated block */
	int word;			/* word in block bitmap */
	uint32_t bit;
	int group;
	char update_bsearch = 0;
	int i;
	struct ext4_group_desc *gd;

	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(nas->node);
	fsi = nas->node->i_sb->sb_data;

	block = EXT4_NO_BLOCK;
	bit = -1;

	if (goal >= fsi->e4sb.s_blocks_count ||
		(goal < fsi->e4sb.s_first_data_block && goal != 0)) {
		goal = fsi->s_bsearch;
	}

	if (goal <= fsi->s_bsearch) {
		/* No reason to search in a place with no free blocks */
		goal = fsi->s_bsearch;
		update_bsearch = DIRTY;
	}

	/* Figure out where to start the bit search. */
	word = ((goal - fsi->e4sb.s_first_data_block) %
			fsi->e4sb.s_blocks_per_group) / EXT4_FS_BITCHUNK_BITS;

	/* Try to allocate block at any group starting from the goal's group.
	* First time goal's group is checked from the word=goal, after all
	* groups checked, it's checked again from word=0, that's why "i <=".
	*/
	group = (goal - fsi->e4sb.s_first_data_block) / fsi->e4sb.s_blocks_per_group;
	for (i = 0; i <= fsi->s_groups_count; i++, group++) {

		if (group >= fsi->s_groups_count) {
			group = 0;
		}
		gd = ext4_get_group_desc(group, fsi);
		if (ext4_free_blocks_count(gd) == 0) {
			word = 0;
			continue;
		}

		ext2_read_sector(nas->node->i_sb, fi->f_buf, 1, ext4_block_bitmap_len(gd));

		bit = ext4_setbit(b_bitmap(fi->f_buf), fsi->e4sb.s_blocks_per_group, word);
		if (-1 == bit) {
			if (0 == word) {
				/* allocator failed to allocate a bit in bitmap	with free bits.*/
				return 0;
			} else {
				word = 0;
				continue;
			}
		}

		block = fsi->e4sb.s_first_data_block + group * fsi->e4sb.s_blocks_per_group + bit;
		if (ext4_check_block_number(block, fsi, gd)) {
			return 0;
		}

		ext2_write_sector(nas->node->i_sb, fi->f_buf, 1, ext4_block_bitmap_len(gd));

		fsi->e4sb.s_free_blocks_count--;
		ext2_write_sblock(nas->node->i_sb);
		ext4_inc_lo_hi(gd->free_blocks_count_lo, gd->free_blocks_count_hi);
		ext2_write_gdblock(nas->node->i_sb);

		if (update_bsearch && block != -1 && block != EXT4_NO_BLOCK) {
			/* We searched from the beginning, update bsearch. */
			fsi->s_bsearch = block;
		}

		return block;
	}

	return block;
}

void ext4_free_block(struct nas *nas, uint32_t bit_returned) {
	/* Return a block by turning off its bitmap bit. */
	int group;		/* group number of bit_returned */
	int bit;		/* bit_returned number within its group */
	struct ext4_group_desc *gd;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(nas->node);
	fsi = nas->node->i_sb->sb_data;

	if (bit_returned >= fsi->e4sb.s_blocks_count ||
		bit_returned < fsi->e4sb.s_first_data_block) {
		return;
	}

	/* At first search group, to which bit_returned belongs to
	* and figure out in what word bit is stored.
	*/
	group = (bit_returned - fsi->e4sb.s_first_data_block) / fsi->e4sb.s_blocks_per_group;
	bit = (bit_returned - fsi->e4sb.s_first_data_block) % fsi->e4sb.s_blocks_per_group;

	gd = ext4_get_group_desc(group, fsi);

	/* We might be buggy (No way! :P), so check if we deallocate
	* data block, but not control (system) block.
	* This should never happen.
	*/
	if (bit_returned == ext4_inode_bitmap_len(gd) || bit_returned == ext4_block_bitmap_len(gd)
		|| (bit_returned >= ext4_inode_table_len(gd)
		&& bit_returned < (ext4_inode_table_len(gd) + fsi->s_itb_per_group))) {
		return;
	}

	ext2_read_sector(nas->node->i_sb, (char *) fi->f_buf, 1, ext4_block_bitmap_len(gd));
	if (ext2_unsetbit(b_bitmap(fi->f_buf), bit)) {
		return; /*Tried to free unused block*/
	}
	ext2_write_sector(nas->node->i_sb, (char *) fi->f_buf, 1, ext4_block_bitmap_len(gd));


	fsi->e4sb.s_free_blocks_count++;
	ext2_write_sblock(nas->node->i_sb);
	ext4_inc_lo_hi(gd->free_blocks_count_lo, gd->free_blocks_count_hi);
	ext2_write_gdblock(nas->node->i_sb);

	if (bit_returned < fsi->s_bsearch) {
		fsi->s_bsearch = bit_returned;
	}
}

uint32_t ext4_alloc_block(struct nas *nas, uint32_t block)
{
	/* Allocate a block for inode. If block is provided, then use it as a goal:
	* try to allocate this block or his neghbors.
	* If block is not provided then goal is group, where inode lives.
	*/
	uint32_t goal;
	uint32_t b;
	int group;
	struct ext4_file_info *fi;
	struct ext4_fs_info *fsi;

	fi = inode_priv(nas->node);
	fsi = nas->node->i_sb->sb_data;

	if (fsi->e4sb.s_free_blocks_count == 0) {
		return EXT4_NO_BLOCK;
	}

	if (block != EXT4_NO_BLOCK) {
		goal = block;
	} else {
		group = (fi->f_num - 1) / fsi->e4sb.s_inodes_per_group;
		goal = fsi->e4sb.s_blocks_per_group * group + fsi->e4sb.s_first_data_block;
	}

	b = ext4_alloc_block_bit(nas, goal);
	if (b != EXT4_NO_BLOCK) {
		fi->f_bsearch = b;
	}
	return b;
}

