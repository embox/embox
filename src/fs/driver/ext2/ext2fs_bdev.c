/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <string.h>

#include <fs/fs_driver.h>

#include <fs/ext2.h>
#include <fs/super_block.h>
#include <fs/journal.h>

#include <drivers/block_dev.h>

/*
 * help function
 */

int ext2_read_sector(struct super_block *sb, char *buffer, uint32_t count,
		uint32_t sector) {
	struct ext2_fs_info *fsi;
	fsi = sb->sb_data;

	if (0 > block_dev_read(sb->bdev, (char *) buffer,
			count * fsi->s_block_size, fsbtodb(fsi, sector))) {
		return -1;
	} else {
		return count;
	}
}

int ext2_write_sector(struct super_block *sb, char *buffer, uint32_t count,
		uint32_t sector) {
	struct ext2_fs_info *fsi;

	fsi = sb->sb_data;

	if (!strcmp(sb->fs_drv->name, "ext3")) {
		/* EXT3 */
		int i = 0;
		journal_t *jp = fsi->journal;
		journal_block_t *b;

		assert(jp);

		for (i = 0 ; i < count; i++) {
			b = journal_new_block(jp, sector + i);
			if (!b) {
				return -1;
			}
			memcpy(b->data, buffer + i * fsi->s_block_size, fsi->s_block_size);
			journal_dirty_block(jp, b);
		}
	} else {
		/* EXT2 */
		if (0 > block_dev_write(sb->bdev, (char *) buffer,
						count * fsi->s_block_size, fsbtodb(fsi, sector))) {
			return -1;
		}
	}

	return count;
}
