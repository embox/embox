/**
 * @file
 *
 * @date Nov 21, 2019
 * @author Anton Bondarev
 */

struct block_dev;
struct dev_module;

int devfs_add_block(struct block_dev *bdev) {
	return 0;
}

int devfs_del_block(struct block_dev *bdev) {
	return 0;
}

int devfs_add_char(struct dev_module *cdev) {
	return 0;
}
