/**
 * @file
 *
 * @date Dec 12, 2019
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <fcntl.h>

#include <fs/file_desc.h>
#include <fs/inode.h>
#include <fs/super_block.h>

#include "fat.h"

static size_t fat_read(struct file_desc *desc, void *buf, size_t size) {
	size_t rezult;
	uint32_t bytecount;
	struct fat_file_info *fi;

	fi = file_get_inode_data(desc);
	fi->pointer = file_get_pos(desc);

	rezult = fat_read_file(fi, fat_sector_buff, buf, &bytecount, size);
	if (DFS_OK == rezult) {
		return bytecount;
	}
	return 0;
}

static size_t fat_write(struct file_desc *desc, void *buf, size_t size) {
	size_t rezult;
	uint32_t bytecount;
	struct fat_file_info *fi;
	size_t new_sz;
	int old_pos = file_get_pos(desc);

	fi = file_get_inode_data(desc);
	fi->pointer = old_pos;
	new_sz = file_get_size(desc);
	fi->mode = O_RDWR; /* XXX */

	rezult = fat_write_file(fi, fat_sector_buff, (uint8_t *)buf,
			&bytecount, size, &new_sz);

	if (DFS_OK == rezult || DFS_EOF == rezult) {
		if (old_pos + bytecount > file_get_size(desc)) {
			file_set_size(desc, old_pos + bytecount);
			fi->filelen = old_pos + bytecount;
		}

		return bytecount;
	}

	return 0;
}

struct file_operations fat_fops = {
	.read = fat_read,
	.write = fat_write,
};

/**
 * @brief Cleanup FS-specific stuff. No need to clean all files: VFS should
 * do it by itself
 *
 * @param sb Pointer to superblock
 *
 * @return Negative error code or 0 if succeed
 */
int fat_clean_sb(struct super_block *sb) {
	struct fat_fs_info *fsi;

	assert(sb);

	fsi = sb->sb_data;

	assert(fsi);

	fat_fs_free(fsi);

	return 0;
}

int fat_delete(struct inode *node) {
	struct fat_file_info *fi;

	fi = inode_priv(node);

	if (fat_unlike_file(fi, (uint8_t *) fat_sector_buff)) {
		return -1;
	}

	if (S_ISDIR(node->i_mode)) {
		fat_dirinfo_free((void *) fi);
	} else {
		fat_file_free(fi);
	}

	return 0;
}
