/**
 * @file
 *
 * @data 23.11.2015
 * @author: Anton Bondarev
 */
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <lib/libds/array.h>

#include <fs/dvfs.h>
#include <kernel/task.h>
#include <embox/cmd.h>
#include <fs/fuse_module.h>

ARRAY_SPREAD_DEF(const struct fuse_module *const, fuse_module_repo);

struct fuse_module *fuse_module_lookup(const char *fuse_type) {
	struct fuse_module *fm;
	array_spread_foreach(fm, fuse_module_repo) {
		if (0 == strcmp(fuse_type, fm->fuse_module_name)) {
			return fm;
		}
	}
	return NULL;
}

struct fuse_mount_params {
	volatile struct fuse_module *fm;
	char *dev;
	char *dest;
};

static int fuse_fill_dentry(struct super_block *sb, char *dest) {
	struct dentry *d;
	struct lookup lookup = {};
	int err;

	if ((err = dvfs_lookup(dest, &lookup))) {
		return err;
	}

	assert(lookup.item);
	assert(lookup.item->flags & S_IFDIR);

	if (!(lookup.item->flags & VFS_DIR_VIRTUAL)) {
		/* Hide dentry of the directory */
		dlist_del(&lookup.item->children_lnk);
		dvfs_cache_del(lookup.item);

		d = dvfs_alloc_dentry();
		dentry_ref_inc(d);

		dentry_fill(sb, NULL, d, lookup.parent);
		strcpy(d->name, lookup.item->name);
	} else {
		d = lookup.item;
		/* TODO free related inode */
	}
	d->flags |= S_IFDIR | DVFS_MOUNT_POINT;
	d->d_sb = sb, d->d_ops = sb ? sb->sb_dops : NULL, dentry_ref_inc(d);
	sb->root = d;

	d->d_inode = dvfs_alloc_inode(sb);
	*d->d_inode = (struct inode ) {
		.i_mode = S_IFDIR,
		.i_ops = sb->sb_iops,
		.i_sb = sb,
		.i_dentry = d,
	};

	return 0;
}

static void *fuse_module_mount_process(void *arg) {
	struct fuse_mount_params *params;
	const struct cmd *cmd;
	struct super_block *sb;
	const struct fs_driver *fs_drv;
	char *argv[3];
	char argv0[0x20];
	char argv1[0x20];
	char argv2[0x20];

	params = arg;

	cmd = cmd_lookup(params->fm->fuse_module_cmd_mount);
	assert(cmd);

	fs_drv = fs_driver_find(params->fm->fuse_module_cmd_mount);
	assert(fs_drv);

	strncpy(argv0, params->fm->fuse_module_cmd_mount, sizeof(argv0));
	strncpy(argv1, params->dev, sizeof(argv1));
	strncpy(argv2, params->dest, sizeof(argv2));

	argv[0] = argv0;
	argv[1] = argv1;
	argv[2] = argv2;

	params->fm = NULL;

	sb = super_block_alloc(fs_drv, NULL);
	fuse_fill_dentry(sb, argv2);

	cmd_exec(cmd, 3, argv); /* will not return */

	return NULL;
}

int fuse_module_mount(struct fuse_module *fm, const char *dev, const char *dest) {
	int res;
	struct fuse_mount_params params = {fm, dev, dest};

	assert(fm);

	res = new_task(fm->fuse_module_cmd_mount, fuse_module_mount_process, &params);
	if (res) {

		return res;
	}
	while (params.fm) {
		sleep(0);
	}

	return 0;
}

int fuse_module_umount(struct fuse_module *fm) {
	return -ENOSUPP;
}

