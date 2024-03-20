/**
 * @brief
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/err.h>
#include <vfs/core.h>

#define ROOTFS OPTION_STRING_GET(rootfs)

EMBOX_UNIT_INIT(rootfs_init);

static struct super_block *rootfs_sb;

struct super_block *vfs_get_rootfs(void) {
	return rootfs_sb;
}

int rootfs_init(void) {
	int err;

	rootfs_sb = vfs_super_block_mount(ROOTFS, NULL, false);
	if ((err = ptr2err(rootfs_sb))) {
		return err;
	}

	return 0;
}
