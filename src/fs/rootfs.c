/**
 * @file
 * @details realize file operation function
 *
 * @date 29.06.09
 * @author Anton Bondarev
 * @author Andrey Gazukin
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include <fs/fs_drv.h>
#include <fs/node.h>
#include <fs/vfs.h>

#include <fs/mount.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

static node_t *root_node;
static struct mount_params mp;

static int rootfs_mount(void *par) {
	fs_drv_t *fsdrv;
	struct mount_params *mp;

	static const char* fs_type = OPTION_STRING_GET(fstype);
	mp = (mount_params_t *) par;

	/* mount dev filesystem */
	if (NULL != (fsdrv = filesystem_find_drv("devfs"))) {
		fsdrv->fsop->mount(NULL);
	}
	/* looking for a device that will be mounted */
	if(0 != *mp->dev) {
		if (NULL == (mp->dev_node = vfs_find_node(mp->dev, NULL))) {
			return -ENODEV;
		}
	}
	/* looking for a driver of root filesystem */
	fsdrv = filesystem_find_drv(fs_type);

	/* mount root filesystem */
	if (NULL != fsdrv) {
		root_node->fs_type = fsdrv;
		return fsdrv->fsop->mount(mp);
	}

	return 0;
}

static int unit_init(void) {

	mp.dev = OPTION_STRING_GET(src);
	mp.dir = OPTION_STRING_GET(dst);
	mp.ext = OPTION_STRING_GET(mntscript);

	if(0 != *mp.dir) {
		root_node = vfs_get_root();
		if (root_node == NULL) {
			return -ENOMEM;
		}

		return rootfs_mount(&mp);
	}
	return -ENOENT;
}
