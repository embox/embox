/**
 * @file
 * @brief
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */


#ifndef EXT2_BALLOC_H_
#define EXT2_BALLOC_H_

#include <stdint.h>

struct inode;
struct super_block;
struct ext2_fs_info;

/* balloc.c */
extern uint32_t ext2_alloc_block(struct inode *node, uint32_t goal);
extern void ext2_free_block(struct inode *node, uint32_t bit);

extern struct ext2_gd* ext2_get_group_desc(unsigned int bnum, struct ext2_fs_info *fsi);

extern uint32_t ext2_setbit(uint32_t *bitmap, uint32_t max_bits, unsigned int word);
extern int ext2_unsetbit(uint32_t *bitmap, uint32_t bit);

#endif /* EXT_H_ */
