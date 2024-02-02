/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <fs/mount.h>

#include <drivers/block_dev.h>

#include <mem/phymem.h>

#include <module/embox/fs/fs_api.h>

static void print_usage(void) {
	printf("Usage: mount [-h] [-t fstype dev dir]\n");
}

#ifdef __MODULE__embox__fs__core__H_

#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/fs_driver.h>

static void lookup_mounts(struct mount_descriptor *parent) {
	struct mount_descriptor *desc;
	char mount_path[PATH_MAX];
	struct path path;

	path.mnt_desc = parent;
	path.node = parent->mnt_root;

	vfs_get_path_by_node(&path, mount_path);
	printf("%s on %s type %s\n",
			parent->mnt_dev[0] ? parent->mnt_dev : "none",
			mount_path,
			parent->mnt_root->i_sb->fs_drv->name);

	dlist_foreach_entry(desc, &parent->mnt_mounts, mnt_child) {
		lookup_mounts(desc);
	}
}

static void show_mount_list(void) {
	struct mount_descriptor *mount_list;

	if (NULL != (mount_list = mount_table())) {
		lookup_mounts(mount_list);
	}
}

#elif defined __MODULE__embox__fs__dvfs__core__H_

#include <fs/dvfs.h>

extern struct dlist_head dentry_dlist;

static void show_mount_list(void) {
	struct dentry *d = NULL;
	static const char *devfs_path = "/dev/";
	char mount_path[PATH_MAX];
	char bdev_path[PATH_MAX];

	dlist_foreach_entry(d, &dentry_dlist, d_lnk) {
		if (d->flags & DVFS_MOUNT_POINT) {
			if (dentry_full_path(d, mount_path)) {
				continue;
			}

			strcpy(bdev_path, devfs_path);
			strncat(bdev_path, d->d_sb->fs_drv->name,
					sizeof(bdev_path) - strlen(devfs_path));

			printf("%s on %s type %s\n",
					bdev_path,
					mount_path,
					d->d_sb->fs_drv->name);
		}
	}
}

#endif

int main(int argc, char **argv) {
	int opt;
	char *dev, *dir;
	char *fs_type = NULL;

	if (argc == 1) {
		show_mount_list();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "ht:"))) {
		switch (opt) {
			case 't':
				fs_type = optarg;
				break;
			case 'h':
				print_usage();
				/* FALLTHROUGH */
			default:
				return 0;
		}
	}

	/* Should be exactly five arguments:
	 *     mount -t fs_type dev dir */
	if (argc != 5 || !fs_type) {
		print_usage();
		return 0;
	}

	dev = argv[argc - 2];
	dir = argv[argc - 1];

	if (0 > mount(dev, dir, fs_type)) {
		return -errno;
	}

	return 0;
}
