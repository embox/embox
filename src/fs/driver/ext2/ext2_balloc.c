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
#include <fs/ext2_super.h>
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

static uint32_t alloc_block_bit(struct nas *nas, struct super_block *sp, uint32_t origin,
	struct inode *rip);

/* help function */
static struct group_desc* get_group_desc(unsigned int bnum, struct super_block *sp) {
	if (bnum >= sp->s_groups_count) {
		return NULL;
	}
	return &sp->s_group_desc[bnum];
}

/*===========================================================================*
 *				setbit   				     *
 *===========================================================================*/
uint32_t setbit(uint32_t *bitmap, uint32_t max_bits, unsigned int word)
{
  /* Find free bit in bitmap and set. Return number of the bit,
   * if failed return -1.
   */
  uint32_t *wptr, *wlim;
  uint32_t b = -1;

  /* TODO: do we need to add 1? I saw a situation, when it was
   * required, and since we check bit number with max_bits it
   * should be safe.
   */
  wlim = &bitmap[FS_BITMAP_CHUNKS(max_bits >> 3)];

  /* Iterate over the words in block. */
  for (wptr = &bitmap[word]; wptr < wlim; wptr++) {
	uint32_t i;
	uint32_t k;

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


/*===========================================================================*
 *				setbyte   				     *
 *===========================================================================*/
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
	if (*wptr | 0)
		continue;

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


/*===========================================================================*
 *				unsetbit   				     *
 *===========================================================================*/
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
  if (!(k & mask))
	return -1;

  k &= ~mask;
  bitmap[word] = (int) k;
  return 0;
}

/*===========================================================================*
 *                      discard_preallocated_blocks                          *
 *===========================================================================*/
void discard_preallocated_blocks(struct nas *nas, struct inode *rip)
{
/* When called for rip, discard (free) blocks preallocated for rip,
 * otherwise discard all preallocated blocks.
 * Normally it should be called in following situations:
 * 1. File is closed.
 * 2. File is truncated.
 * 3. Non-sequential write.
 * 4. inode is "unloaded" from the memory.
 * 5. No free blocks left (discard all preallocated blocks).
 */
  int i;

  if (rip) {
	rip->i_prealloc_count = rip->i_prealloc_index = 0;
	for (i = 0; i < EXT2_PREALLOC_BLOCKS; i++) {
		if (rip->i_prealloc_blocks[i] != NO_BLOCK) {
			free_block(nas, rip->i_sp, rip->i_prealloc_blocks[i]);
			rip->i_prealloc_blocks[i] = NO_BLOCK;
		}
	}
	return;
  }

  /* Discard all allocated blocks.
   * Probably there are just few blocks on the disc, so forbid preallocation.
  for(rip = &inode[0]; rip < &inode[NR_INODES]; rip++) {
	rip->i_prealloc_count = rip->i_prealloc_index = 0;
	rip->i_preallocation = 0; // forbid preallocation /
	for (i = 0; i < EXT2_PREALLOC_BLOCKS; i++) {
		if (rip->i_prealloc_blocks[i] != NO_BLOCK) {
			free_block(nas, rip->i_sp, rip->i_prealloc_blocks[i]);
			rip->i_prealloc_blocks[i] = NO_BLOCK;
		}
	}
  }*/
}


/*===========================================================================*
 *                              alloc_block                                  *
 *===========================================================================*/
uint32_t alloc_block(struct nas *nas, struct inode *rip, uint32_t block)
{
/* Allocate a block for inode. If block is provided, then use it as a goal:
 * try to allocate this block or his neghbors.
 * If block is not provided then goal is group, where inode lives.
 */
  uint32_t goal;
  uint32_t b;
  struct super_block *sp = rip->i_sp;

  /*if (sp->s_rd_only) {
	  panic("can't alloc block on read-only filesys.");
  }*/

  /* Check for free blocks. First time discard preallocation,
   * next time return NO_BLOCK
   */
  /*if (!opt.use_reserved_blocks &&
      sp->s_free_blocks_count <= sp->s_r_blocks_count) {
	  discard_preallocated_blocks(nas, NULL);
  }
  else */
	if (sp->s_free_blocks_count <= EXT2_PREALLOC_BLOCKS) {
		discard_preallocated_blocks(nas, NULL);
	}

  /*if (!opt.use_reserved_blocks &&
      sp->s_free_blocks_count <= sp->s_r_blocks_count) {
	  return(NO_BLOCK);
  } else */
	  if (sp->s_free_blocks_count == 0) {
	  return(NO_BLOCK);
  }

  if (block != NO_BLOCK) {
	goal = block;
	if (rip->i_preallocation && rip->i_prealloc_count > 0) {
		/* check if goal is preallocated */
		b = rip->i_prealloc_blocks[rip->i_prealloc_index];
		if (block == b || (block + 1) == b) {
			/* use preallocated block */
			rip->i_prealloc_blocks[rip->i_prealloc_index] = NO_BLOCK;
			rip->i_prealloc_count--;
			rip->i_prealloc_index++;
			if (rip->i_prealloc_index >= EXT2_PREALLOC_BLOCKS) {
				rip->i_prealloc_index = 0;
				//ASSERT(rip->i_prealloc_count == 0);
			}
			rip->i_bsearch = b;
			return b;
		} else {
			/* probably non-sequential write operation,
			 * disable preallocation for this inode.
			 */
			rip->i_preallocation = 0;
			discard_preallocated_blocks(nas, rip);
		}
	}
  }
  else {
	  int group = (rip->i_num - 1) / sp->s_inodes_per_group;
	  goal = sp->s_blocks_per_group*group + sp->s_first_data_block;
  }

  if (rip->i_preallocation && rip->i_prealloc_count) {
	//ext2_debug("There're preallocated blocks, but they'reneither used or freed!");
  }

  b = alloc_block_bit(nas, sp, goal, rip);

  if (b != NO_BLOCK)
	rip->i_bsearch = b;

  return b;
}


static void check_block_number(uint32_t block, struct super_block *sp,
	struct group_desc *gd);


static uint32_t alloc_block_bit(struct nas *nas,
								struct super_block *sp,		/* the filesystem to allocate from */
									uint32_t goal,			/* try to allocate near this block */
									struct inode *rip) {    /* used for preallocation */
  uint32_t block = NO_BLOCK;	/* allocated block */
  int word;			/* word in block bitmap */
  uint32_t	bit = -1;
  int group;
  char update_bsearch = 0;
  int i;

  if (goal >= sp->s_blocks_count ||
      (goal < sp->s_first_data_block && goal != 0)) {
	goal = sp->s_bsearch;
  }

  if (goal <= sp->s_bsearch) {
	/* No reason to search in a place with no free blocks */
	goal = sp->s_bsearch;
	update_bsearch = DIRTY;
  }

  /* Figure out where to start the bit search. */
  word = ((goal - sp->s_first_data_block) % sp->s_blocks_per_group)
			/ FS_BITCHUNK_BITS;

  /* Try to allocate block at any group starting from the goal's group.
   * First time goal's group is checked from the word=goal, after all
   * groups checked, it's checked again from word=0, that's why "i <=".
   */
  group = (goal - sp->s_first_data_block) / sp->s_blocks_per_group;
  for (i = 0; i <= sp->s_groups_count; i++, group++) {
	struct buf buff,*bp;
	struct group_desc *gd;
	bp = &buff;

	if (group >= sp->s_groups_count)
		group = 0;

	gd = get_group_desc(group, sp);
	/*if (gd == NULL)
		panic("can't get group_desc to alloc block");*/

	if (gd->free_blocks_count == 0) {
		word = 0;
		continue;
	}

	//bp = get_block(sp->s_dev, gd->block_bitmap, NORMAL);
	ext2_read_sector(nas, (char *) bp->data, 1, gd->block_bitmap);

	if (rip->i_preallocation &&
	    gd->free_blocks_count >= (EXT2_PREALLOC_BLOCKS * 4) ) {
		/* Try to preallocate blocks */
		if (rip->i_prealloc_count != 0) {
			/* kind of glitch... */
			discard_preallocated_blocks(nas, rip);
			/*ext2_debug("warning, discarding previously preallocated
				    blocks! It had to be done by another code.");*/
		}
		//ASSERT(rip->i_prealloc_count == 0);
		/* we preallocate bytes only */
		//ASSERT(EXT2_PREALLOC_BLOCKS == sizeof(char) * CHAR_BIT);

		bit = setbyte(b_bitmap(bp), sp->s_blocks_per_group);
		if (bit != -1) {
			block = bit + sp->s_first_data_block +
					group * sp->s_blocks_per_group;
			check_block_number(block, sp, gd);

			/* We preallocate a byte starting from block.
			 * First preallocated block will be returned as
			 * normally allocated block.
			 */
			for (i = 1; i < EXT2_PREALLOC_BLOCKS; i++) {
				check_block_number(block + i, sp, gd);
				rip->i_prealloc_blocks[i-1] = block + i;
			}
			rip->i_prealloc_index = 0;
			rip->i_prealloc_count = EXT2_PREALLOC_BLOCKS - 1;

			bp->lmfs_dirt = DIRTY; /* by setbyte */
			//put_block(bp, MAP_BLOCK);

			gd->free_blocks_count -= EXT2_PREALLOC_BLOCKS;
			sp->s_free_blocks_count -= EXT2_PREALLOC_BLOCKS;
			group_descriptors_dirty = DIRTY;
			return block;
		}
	}

        bit = setbit(b_bitmap(bp), sp->s_blocks_per_group, word);
	if (bit == -1) {
		if (word == 0) {
			/*panic("ext2: allocator failed to allocate a bit in bitmap
				with free bits.");*/
		} else {
			word = 0;
			continue;
		}
	}

	block = sp->s_first_data_block + group * sp->s_blocks_per_group + bit;
	check_block_number(block, sp, gd);

        bp->lmfs_dirt = DIRTY; /* Now it's safe to mark it as dirty */
	//put_block(bp, MAP_BLOCK);

	gd->free_blocks_count--;
	sp->s_free_blocks_count--;
	group_descriptors_dirty = DIRTY;

	if (update_bsearch && block != -1 && block != NO_BLOCK) {
		/* We searched from the beginning, update bsearch. */
		sp->s_bsearch = block;
	}

	return block;
  }

  return block;
}


/*===========================================================================*
 *                        free_block	                                     *
 *===========================================================================*/
void free_block(struct nas *nas, struct super_block *sp, uint32_t bit_returned)
{
/* Return a block by turning off its bitmap bit. */
  int group;		/* group number of bit_returned */
  //int bit;		/* bit_returned number within its group */
  struct buf buff, *bp;
  struct group_desc *gd;

  bp = &buff;

  if (sp->s_rd_only)
	//panic("can't free bit on read-only filesys.");

  if (bit_returned >= sp->s_blocks_count ||
      bit_returned < sp->s_first_data_block)
	/*panic("trying to free block %d beyond blocks scope.",
		bit_returned);*/

  /* At first search group, to which bit_returned belongs to
   * and figure out in what word bit is stored.
   */
  group = (bit_returned - sp->s_first_data_block) / sp->s_blocks_per_group;
  //bit = (bit_returned - sp->s_first_data_block) % sp->s_blocks_per_group;

  gd = get_group_desc(group, sp);

  /* We might be buggy (No way! :P), so check if we deallocate
   * data block, but not control (system) block.
   * This should never happen.
   */
  if (bit_returned == gd->inode_bitmap || bit_returned == gd->block_bitmap
      || (bit_returned >= gd->inode_table
          && bit_returned < (gd->inode_table + sp->s_itb_per_group))) {
	/*ext2_debug("ext2: freeing non-data block %d\n", bit_returned);
	panic("trying to deallocate
		system/control block, hardly poke author.");*/
  }

  //bp = get_block(sp->s_dev, gd->block_bitmap, NORMAL);
  ext2_read_sector(nas, (char *) bp->data, 1, gd->block_bitmap);


  /*if (unsetbit(b_bitmap(bp), bit))
	panic("Tried to free unused block", bit_returned);*/

  bp->lmfs_dirt = DIRTY;
  //put_block(bp, MAP_BLOCK);

  gd->free_blocks_count++;
  sp->s_free_blocks_count++;

  group_descriptors_dirty = DIRTY;

  if (bit_returned < sp->s_bsearch)
	sp->s_bsearch = bit_returned;
}


static void check_block_number(uint32_t block, struct super_block *sp,
				struct group_desc *gd)
{

  /* Check if we allocated a data block, but not control (system) block.
   * Only major bug can cause us to allocate wrong block. If it happens,
   * we panic (and don't bloat filesystem's bitmap).
   */
  if (block == gd->inode_bitmap || block == gd->block_bitmap ||
      (block >= gd->inode_table
       && block < (gd->inode_table + sp->s_itb_per_group))) {
	/*ext2_debug("ext2: allocating non-data block %d\n", block);
	 * panic("ext2: block allocator tryed to return
	 * system/control block, poke author.\n");
	 */
  }

  if (block >= sp->s_blocks_count) {
	/*panic("ext2: allocator returned blocknum greater, than
			total number of blocks.\n");*/
  }
}
