/**
 * @file
 * @details realize file operation function
 *
 * @date 29.06.09
 * @author Anton Bondarev
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

	if (NULL != (fsdrv = filesystem_find_drv("devfs"))) {
		fsdrv->fsop->mount(NULL);
	}
	fsdrv = filesystem_find_drv(fs_type);

	if(0 != *mp->dev) {
		if (NULL == (mp->dev_node = vfs_find_node(mp->dev, NULL))) {
			if(NULL != (mp->dev_node = vfs_add_path(mp->dev, NULL))) {
				mp->dev_node->fs_type = fsdrv;
			}
		}
	}

	if (NULL != fsdrv) {
		root_node->fs_type = fsdrv;
		fsdrv->fsop->mount(mp);
	}

	return 0;
}

static int unit_init(void) {

	mp.dev = OPTION_STRING_GET(src);
	mp.dir = OPTION_STRING_GET(dst);
	mp.ext = OPTION_STRING_GET(mntscript);

	if(0 != *mp.dir) {
		root_node = get_root_node();
		if (root_node == NULL) {
			return -ENOMEM;
		}

		return rootfs_mount(&mp);
	}
	return -ENOENT;
}
