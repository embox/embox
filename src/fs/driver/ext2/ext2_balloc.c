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

#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <fs/ext2.h>
#include <fs/path.h>
#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <drivers/ramdisk.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>

int group_descriptors_dirty;

static uint32_t alloc_block_bit(struct nas *nas, uint32_t origin);

/* help function */
struct ext2_gd* get_group_desc(unsigned int bnum, struct ext2_fs_info *fsi) {
	if (bnum >= fsi->s_groups_count) {
		return NULL;
	}
	return &fsi->e2fs_gd[bnum];
}

uint32_t setbit(uint32_t *bitmap, uint32_t max_bits, unsigned int word)
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
		if (*wptr == (uint32_t) ~0)
			continue;

		/* Find and allocate the free bit. */
		k = (int) *wptr;
		for (i = 0; (k & (1 << i)) != 0; ++i) {}

		/* Bit number from the start of the bit map. */
		b = (wptr - &bitmap[0]) * FS_BITCHUNK_BITS + i;

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


uint32_t setbyte(uint32_t *bitmap, uint32_t max_bits)
{
	/* Find free byte in bitmap and set it. Return number of the starting bit,
	* if failed return -1.
	*/
	unsigned char *wptr, *wlim;
	uint32_t b = -1;

	wptr = (unsigned char*) &bitmap[0];
	/* TODO: do we need to add 1? I saw a situation, when it was
	* required, and since we check bit number with max_bits it
	* should be safe.
	*/
	wlim = &wptr[(max_bits >> 3)];

	/* Iterate over the words in block. */
	for ( ; wptr < wlim; wptr++) {
		/* Is it a free byte? */
		if (*wptr | 0) {
			continue;
		}

		/* Bit number from the start of the bit map. */
		b = (wptr - (unsigned char*) &bitmap[0]) * CHAR_BIT;

		/* Don't allocate bits beyond the end of the map. */
		if (b + CHAR_BIT >= max_bits) {
			b = -1;
			continue;
		}

		/* Allocate byte number. */
		*wptr = (unsigned char) ~0;
		break;
	}
	return b;
}


int unsetbit(uint32_t *bitmap, uint32_t bit)
{
	/* Unset specified bit. If requested bit is already free return -1,
	* otherwise return 0.
	*/
	unsigned int word;		/* bit_returned word in bitmap */
	uint32_t k, mask;

	word = bit / FS_BITCHUNK_BITS;
	bit = bit % FS_BITCHUNK_BITS; /* index in word */
	mask = 1 << bit;

	k = (int) bitmap[word];
	if (!(k & mask)) {
		return -1;
	}

	k &= ~mask;
	bitmap[word] = (int) k;
	return 0;
}

void discard_preallocated_blocks(struct nas *nas)
{
	/* When called for fi, discard (free) blocks preallocated for fi,
	* otherwise discard all preallocated blocks.
	* Normally it should be called in following situations:
	* 1. File is closed.
	* 2. File is truncated.
	* 3. Non-sequential write.
	* 4. inode is "unloaded" from the memory.
	* 5. No free blocks left (discard all preallocated blocks).
	*/
	int i;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	if (fi) {
		fi->i_prealloc_count = fi->i_prealloc_index = 0;
		for (i = 0; i < EXT2_PREALLOC_BLOCKS; i++) {
			if (fi->i_prealloc_blocks[i] != NO_BLOCK) {
				free_block(nas, fsi, fi->i_prealloc_blocks[i]);
				fi->i_prealloc_blocks[i] = NO_BLOCK;
			}
		}
		return;
	}

	/* Discard all allocated blocks.
	* Probably there are just few blocks on the disc, so forbid preallocation.
	for(fi = &inode[0]; fi < &inode[NR_INODES]; fi++) {
		fi->i_prealloc_count = fi->i_prealloc_index = 0;
		fi->i_preallocation = 0; // forbid preallocation /
		for (i = 0; i < EXT2_PREALLOC_BLOCKS; i++) {
			if (fi->i_prealloc_blocks[i] != NO_BLOCK) {
				free_block(nas, fi->i_sp, fi->i_prealloc_blocks[i]);
				fi->i_prealloc_blocks[i] = NO_BLOCK;
			}
		}
	}*/
}

uint32_t alloc_block(struct nas *nas, uint32_t block)
{
	/* Allocate a block for inode. If block is provided, then use it as a goal:
	* try to allocate this block or his neghbors.
	* If block is not provided then goal is group, where inode lives.
	*/
	uint32_t goal;
	uint32_t b;
	int group;
	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	/* Check for free blocks. First time discard preallocation,
	* next time return NO_BLOCK
	*/
	/*if (!opt.use_reserved_blocks &&
		fsi->e2sb.s_free_blocks_count <= fsi->e2sb.s_r_blocks_count) {
		discard_preallocated_blocks(nas, NULL);
	}
	else */
	if (fsi->e2sb.s_free_blocks_count <= EXT2_PREALLOC_BLOCKS) {
		discard_preallocated_blocks(nas);
	}

	/*if (!opt.use_reserved_blocks &&
		fsi->e2sb.s_free_blocks_count <= fsi->e2sb.s_r_blocks_count) {
		return(NO_BLOCK);
	} else */
	if (fsi->e2sb.s_free_blocks_count == 0) {
		return(NO_BLOCK);
	}

	if (block != NO_BLOCK) {
		goal = block;
		if (fi->i_preallocation && fi->i_prealloc_count > 0) {
			/* check if goal is preallocated */
			b = fi->i_prealloc_blocks[fi->i_prealloc_index];
			if (block == b || (block + 1) == b) {
				/* use preallocated block */
				fi->i_prealloc_blocks[fi->i_prealloc_index] = NO_BLOCK;
				fi->i_prealloc_count--;
				fi->i_prealloc_index++;
				if (fi->i_prealloc_index >= EXT2_PREALLOC_BLOCKS) {
					fi->i_prealloc_index = 0;
				}
				fi->i_bsearch = b;
				return b;
			}
			else {
				/* probably non-sequential write operation,
				 * disable preallocation for this inode.
				 */
				fi->i_preallocation = 0;
				discard_preallocated_blocks(nas);
			}
		}
	}
	else {
		group = (fi->i_num - 1) / fsi->e2sb.s_inodes_per_group;
		goal = fsi->e2sb.s_blocks_per_group * group + fsi->e2sb.s_first_data_block;
	}

	if (fi->i_preallocation && fi->i_prealloc_count) {
		//ext2_debug("There're preallocated blocks, but they'reneither used or freed!");
	}
	b = alloc_block_bit(nas, goal);
	if (b != NO_BLOCK) {
		fi->i_bsearch = b;
	}
	return b;
}


static void check_block_number(uint32_t block, struct ext2_fs_info *fsi,
	struct ext2_gd *gd);

static uint32_t alloc_block_bit(struct nas *nas, uint32_t goal) { /* try to allocate near this block */
	uint32_t block;	/* allocated block */
	int word;			/* word in block bitmap */
	uint32_t bit;
	int group;
	char update_bsearch = 0;
	int i;
	struct buf buff,*bp;
	struct ext2_gd *gd;

	struct ext2_file_info *fi;
	struct ext2_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	block = NO_BLOCK;
	bit = -1;

	if (goal >= fsi->e2sb.s_blocks_count ||
		(goal < fsi->e2sb.s_first_data_block && goal != 0)) {
		goal = fsi->s_bsearch;
	}

	if (goal <= fsi->s_bsearch) {
		/* No reason to search in a place with no free blocks */
		goal = fsi->s_bsearch;
		update_bsearch = DIRTY;
	}

	/* Figure out where to start the bit search. */
	word = ((goal - fsi->e2sb.s_first_data_block) %
			fsi->e2sb.s_blocks_per_group) / FS_BITCHUNK_BITS;

	/* Try to allocate block at any group starting from the goal's group.
	* First time goal's group is checked from the word=goal, after all
	* groups checked, it's checked again from word=0, that's why "i <=".
	*/
	group = (goal - fsi->e2sb.s_first_data_block) / fsi->e2sb.s_blocks_per_group;
	for (i = 0; i <= fsi->s_groups_count; i++, group++) {

		bp = &buff;
		if (group >= fsi->s_groups_count) {
			group = 0;
		}

		gd = get_group_desc(group, fsi);

		if (gd->free_blocks_count == 0) {
			word = 0;
			continue;
		}

		//bp = get_block(fsi->s_dev, gd->block_bitmap, NORMAL);
		ext2_read_sector(nas, (char *) bp->data, 1, gd->block_bitmap);

		if (fi->i_preallocation &&
		gd->free_blocks_count >= (EXT2_PREALLOC_BLOCKS * 4) ) {
			/* Try to preallocate blocks */
			if (fi->i_prealloc_count != 0) {
				/* kind of glitch... */
				discard_preallocated_blocks(nas);
				/*ext2_debug("warning, discarding previously preallocated
				blocks! It had to be done by another code.");*/
			}
			/* we preallocate bytes only */
			bit = setbyte(b_bitmap(bp), fsi->e2sb.s_blocks_per_group);
			if (bit != -1) {
				block = bit + fsi->e2sb.s_first_data_block +
				group * fsi->e2sb.s_blocks_per_group;
				check_block_number(block, fsi, gd);

				/* We preallocate a byte starting from block.
				* First preallocated block will be returned as
				* normally allocated block.
				*/
				for (i = 1; i < EXT2_PREALLOC_BLOCKS; i++) {
					check_block_number(block + i, fsi, gd);
					fi->i_prealloc_blocks[i-1] = block + i;
				}
				fi->i_prealloc_index = 0;
				fi->i_prealloc_count = EXT2_PREALLOC_BLOCKS - 1;

				bp->lmfs_dirt = DIRTY; /* by setbyte */
				//put_block(bp, MAP_BLOCK);

				gd->free_blocks_count -= EXT2_PREALLOC_BLOCKS;
				fsi->e2sb.s_free_blocks_count -= EXT2_PREALLOC_BLOCKS;
				group_descriptors_dirty = DIRTY;
				return block;
			}
		}

		bit = setbit(b_bitmap(bp), fsi->e2sb.s_blocks_per_group, word);
		if (bit == -1) {
			if (word == 0) {
					/*panic("ext2: allocator failed to allocate a bit in bitmap
					with free bits.");*/
				}
			else {
				word = 0;
				continue;
			}
		}

		block = fsi->e2sb.s_first_data_block + group * fsi->e2sb.s_blocks_per_group + bit;
		check_block_number(block, fsi, gd);

		bp->lmfs_dirt = DIRTY; /* Now it's safe to mark it as dirty */
		//put_block(bp, MAP_BLOCK);

		gd->free_blocks_count--;
		fsi->e2sb.s_free_blocks_count--;
		group_descriptors_dirty = DIRTY;

		if (update_bsearch && block != -1 && block != NO_BLOCK) {
			/* We searched from the beginning, update bsearch. */
			fsi->s_bsearch = block;
		}

		return block;
	}

	return block;
}

void free_block(struct nas *nas, struct ext2_fs_info *fsi, uint32_t bit_returned)
{
	/* Return a block by turning off its bitmap bit. */
	int group;		/* group number of bit_returned */
	//int bit;		/* bit_returned number within its group */
	struct buf buff, *bp;
	struct ext2_gd *gd;

	bp = &buff;

	if (bit_returned >= fsi->e2sb.s_blocks_count ||
		bit_returned < fsi->e2sb.s_first_data_block) {
		/*panic("trying to free block %d beyond blocks scope.",
		bit_returned);*/
	}

	/* At first search group, to which bit_returned belongs to
	* and figure out in what word bit is stored.
	*/
	group = (bit_returned - fsi->e2sb.s_first_data_block) / fsi->e2sb.s_blocks_per_group;
	//bit = (bit_returned - fsi->e2sb.s_first_data_block) % fsi->e2sb.s_blocks_per_group;

	gd = get_group_desc(group, fsi);

	/* We might be buggy (No way! :P), so check if we deallocate
	* data block, but not control (system) block.
	* This should never happen.
	*/
	if (bit_returned == gd->inode_bitmap || bit_returned == gd->block_bitmap
		|| (bit_returned >= gd->inode_table
		&& bit_returned < (gd->inode_table + fsi->s_itb_per_group))) {
		/*ext2_debug("ext2: freeing non-data block %d\n", bit_returned);
		panic("trying to deallocate
		system/control block, hardly poke author.");*/
	}

	ext2_read_sector(nas, (char *) bp->data, 1, gd->block_bitmap);

	bp->lmfs_dirt = DIRTY;
	//put_block(bp, MAP_BLOCK);

	gd->free_blocks_count++;
	fsi->e2sb.s_free_blocks_count++;

	group_descriptors_dirty = DIRTY;

	if (bit_returned < fsi->s_bsearch) {
		fsi->s_bsearch = bit_returned;
	}
}


static void check_block_number(uint32_t block, struct ext2_fs_info *fsi,
				struct ext2_gd *gd)
{
	/* Check if we allocated a data block, but not control (system) block.
	* Only major bug can cause us to allocate wrong block. If it happens,
	* we panic (and don't bloat filesystem's bitmap).
	*/
	if (block == gd->inode_bitmap || block == gd->block_bitmap ||
		(block >= gd->inode_table
		&& block < (gd->inode_table + fsi->s_itb_per_group))) {
		/*ext2_debug("ext2: allocating non-data block %d\n", block);
		* panic("ext2: block allocator tryed to return
		* system/control block, poke author.\n");
		*/
	}

	if (block >= fsi->e2sb.s_blocks_count) {
		/*panic("ext2: allocator returned blocknum greater, than
		total number of blocks.\n");*/
	}
}
