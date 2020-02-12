/**
 * @file fs_driver.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.02.2020
 */

#ifndef FS_DRV_H_
#define FS_DRV_H_

#define FS_DRV_NAME_LEN   16

struct block_dev;
struct super_block;
struct file_desc;

struct fs_driver {
	const char name[FS_DRV_NAME_LEN];
	int (*format)(struct block_dev *dev, void *priv);
	int (*fill_sb)(struct super_block *sb, const char *source);
	int (*clean_sb)(struct super_block *sb);
};

extern const struct fs_driver *fs_driver_find(const char *name);

#endif /* FS_DRV_H */
