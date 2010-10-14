/**
 * @file
 * @brief Device file system
 *
 * @date 14.10.10
 * @author Nikolay Korotky
 */

#include <fs/fs.h>

static int devfs_init(void) {
//	return register_filesystem(&devfs_drv);
}

static int devfs_create(void *params) {
        return 0;
}

static int devfs_delete(const char *fname) {
        return 0;
}

static void *devfs_open(const char *fname, const char *mode) {
        return NULL;
}

static int devfs_close(FILE *file) {
        return 0;
}

static fsop_desc_t devfs_fsop = {
        devfs_init,
        devfs_create,
        devfs_delete
};

static file_op_t devfs_fop = {
        devfs_open,
        devfs_close,
        NULL,
        NULL,
        NULL,
        NULL
};

static file_system_driver_t devfs_drv = {
	"devfs",
	&devfs_fop,
	&devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_drv);

