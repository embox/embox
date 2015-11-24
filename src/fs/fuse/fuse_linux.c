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

#include <util/array.h>

#include <kernel/task.h>
#include <embox/cmd.h>
#include <fs/fuse_module.h>

ARRAY_SPREAD_DEF(struct fuse_module *, fuse_module_repo);

struct fuse_module *fuse_module_lookup(char *fuse_type) {
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

static void *fuse_module_mount_process(void *arg) {
	struct fuse_mount_params *params;
	const struct cmd *cmd;
	char *argv[3];
	char argv0[0x20];
	char argv1[0x20];
	char argv2[0x20];

	params = arg;

	cmd = cmd_lookup(params->fm->fuse_module_cmd_mount);
	assert(cmd);

	strncpy(argv0, params->fm->fuse_module_cmd_mount, sizeof(argv0));
	strncpy(argv1, params->dev, sizeof(argv1));
	strncpy(argv2, params->dest, sizeof(argv2));

	argv[0] = argv0;
	argv[1] = argv1;
	argv[2] = argv2;

	params->fm = NULL;
	cmd_exec(cmd, 3, argv);
	return NULL;
}

int fuse_module_mount(struct fuse_module *fm, char *dev, char *dest) {
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

