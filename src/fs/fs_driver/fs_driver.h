/**
 * @file
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_DRV_H_
#define FS_DRV_H_

#include <lib/libds/array.h>

struct block_dev;
struct super_block;

/**
 * Structure of file system driver.
 * We can mount some file system with name of FS which has been registered in
 * our system.
 */
struct fs_driver {
	const char                   *name;

	int (*format)(struct block_dev *bdev, void *priv);
	int (*fill_sb)(struct super_block *sb, const char *source);
	int (*clean_sb)(struct super_block *sb);
};

#define DECLARE_FILE_SYSTEM_DRIVER(fs_driver_)      \
	ARRAY_SPREAD_DECLARE(const struct fs_driver *const, fs_drivers_registry); \
	ARRAY_SPREAD_ADD(fs_drivers_registry, &fs_driver_)

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

#endif /* FS_DRV_H_ */
