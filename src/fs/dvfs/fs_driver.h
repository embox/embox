/**
 * @file fs_driver.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.02.2020
 */

#ifndef FS_DRV_H_
#define FS_DRV_H_

#include <util/array.h>

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

#define DECLARE_FILE_SYSTEM_DRIVER(fs_driver_)      \
	ARRAY_SPREAD_DECLARE(const struct fs_driver *const, \
			fs_drivers_registry);                \
	ARRAY_SPREAD_ADD(fs_drivers_registry, \
			&fs_driver_)


struct auto_mount {
	const char *mount_path;
	const struct fs_driver *fs_driver;
};

#define FILE_SYSTEM_AUTOMOUNT(path, fsdriver) \
	static const struct auto_mount fsdriver ## _auto_mount = { \
			.mount_path = path, \
			.fs_driver  = &fsdriver,  \
		}; \
		ARRAY_SPREAD_DECLARE(const struct auto_mount *const, auto_mount_tab); \
		ARRAY_SPREAD_ADD(auto_mount_tab, &fsdriver ## _auto_mount)


extern const struct fs_driver *fs_driver_find(const char *name);

#endif /* FS_DRV_H */
