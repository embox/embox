/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>

#if 0
#include <drivers/block_dev.h>
#include <fs/mount.h>
#include <mem/phymem.h>

#include <module/embox/fs/fs_api.h>

#ifdef __MODULE__embox__fs__core__H_

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/vfs.h>

static void lookup_mounts(struct mount_descriptor *parent) {
	struct mount_descriptor *desc;
	char mount_path[PATH_MAX];
	struct path path;

	path.mnt_desc = parent;
	path.node = parent->mnt_root;

	vfs_get_path_by_node(&path, mount_path);
	printf("%s on %s type %s\n", parent->mnt_dev[0] ? parent->mnt_dev : "none",
	    mount_path, parent->mnt_root->i_sb->fs_drv->name);

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

			printf("%s on %s type %s\n", bdev_path, mount_path,
			    d->d_sb->fs_drv->name);
		}
	}
}

#endif
#endif

static void print_usage(void) {
	printf("Usage: mount [-h] [-t fstype dev dir]\n");
}

int main(int argc, char **argv) {
	char *source;
	char *target;
	char *fs_type;
	long flags;
	int opt;

	flags = 0;
	fs_type = NULL;

	if (argc < 2) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc, argv, "hbt:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'b':
			flags |= MS_BIND;
			break;
		case 't':
			fs_type = optarg;
			break;
		default:
			printf("mount: invalid option -- '%c'\n", optopt);
			return -EINVAL;
		}
	}

	if (optind + 2 != argc) {
		print_usage();
		return -EINVAL;
	}

	source = argv[optind++];
	target = argv[optind];

	if (-1 == mount(source, target, fs_type, flags, NULL)) {
		return -errno;
	}

	return 0;
}
