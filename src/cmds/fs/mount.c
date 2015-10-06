/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <fs/mount.h>

#include <drivers/block_dev.h>

#include <mem/phymem.h>

static void print_usage(void) {
	printf("Usage: mount [-h] [-t fstype] dev dir\n");
}

/* static void lookup_mounts(struct mount_descriptor *parent) {
	struct mount_descriptor *desc;
	char mount_path[PATH_MAX];
	struct path path;

	path.mnt_desc = parent;
	path.node = parent->mnt_root;

	vfs_get_path_by_node(&path, mount_path);
	printf("%s on %s type %s\n",
			parent->mnt_dev[0] ? parent->mnt_dev : "none",
			mount_path,
			parent->mnt_root->nas->fs->drv->name);

	dlist_foreach_entry(desc, &parent->mnt_mounts, mnt_child) {
		lookup_mounts(desc);
	}
}

static void show_mount_list(void) {
	struct mount_descriptor *mount_list;

	if (NULL != (mount_list = mount_table())) {
		lookup_mounts(mount_list);
	}
} */

int main(int argc, char **argv) {
	int opt;
	int opt_cnt;
	char *dev, *dir;
	char *fs_type;

	opt_cnt = 0;
	fs_type = 0;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "ht:"))) {
		switch (opt) {
		case 't':
			opt_cnt++;
			break;
		case '?':
			break;
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (argc > 2) {
		dev = argv[argc - 2];
		dir = argv[argc - 1];
		if (argc > 3) {
			if (opt_cnt) {
				fs_type = argv[argc - 3];
			} else {
				print_usage();
				return 0;
			}
		}

		if (0 == fs_type) {
			print_usage();
			printf("try to set \"-t [fstype]\" option\n");
			return 0;
		}

		if(0 > mount(dev, dir, fs_type)) {
			return -errno;
		}
		return 0;
	}

	//show_mount_list();

	return 0;
}
