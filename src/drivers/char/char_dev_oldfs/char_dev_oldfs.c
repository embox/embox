/*
 * @file
 *
 * @date 28.11.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

#include <drivers/char_dev.h>
#include <fs/file_desc.h>
#include <fs/inode.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>

#include <util/array.h>
#include <util/err.h>
#include <util/log.h>

struct idesc *char_dev_open(struct inode *node, int flags) {
	struct dev_module *cdev = node->nas->fi->privdata;

	if (!cdev) {
		log_error("Can't open char device");
		return NULL;
	}

	if (cdev->dev_open != NULL) {
		return cdev->dev_open(cdev, cdev->dev_priv);
	}

	return char_dev_idesc_create(cdev);
}
