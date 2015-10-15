/**
 * @file
 * @brief DVFS-specific bdev handling
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-01
 */

#include <string.h>

#include <drivers/block_dev.h>
#include <fs/dvfs.h>

/**
 * @brief Create node in devfs
 *
 * @param path Name of bdev
 * @param driver
 * @param privdata
 *
 * @return Pointer to created device or NULL if failed
 */
struct block_dev *block_dev_create(char *path, void *driver, void *privdata) {
	block_dev_t *bdev;
	char full_path[256];
	struct lookup lu;

	if (NULL == (bdev = block_dev_create_common(path, driver, privdata)))
		return NULL;

	/* Get root of devfs in smarter way? */

	strcpy(full_path, "/dev/");
	strcat(full_path, path);

	dvfs_lookup("/dev", &lu);
	lu.parent = lu.item;
	lu.item = NULL;

	if (!lu.parent) {
		block_dev_free(bdev);
		return NULL;
	}

	return bdev;
}

/**
 * @brief Destroy corresponding devfs node
 *
 * @param dev Pointer to device
 *
 * @return Negative error code or 0 if succeed
 *
 * @note Currently it's stub
 */
int block_dev_destroy(void *dev) {
	return 0;
}
