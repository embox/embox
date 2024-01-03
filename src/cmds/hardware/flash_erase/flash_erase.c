/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.12.23
 */
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include <drivers/flash/flash.h>

struct block_dev;
extern struct block_dev *bdev_by_path(const char *source);

static void flash_erase_print_usage(void) {
	printf("Usage: flash_erase [FLASH_DEV]\n");
}

int main(int argc, char **argv) {
	struct flash_dev *fdev;
	struct block_dev *bdev;
	int err;
	int i;

	err = 0;

	if (argc != 2) {
		err = -EINVAL;
		goto out;
	}

	bdev = bdev_by_path(argv[1]);
	if (bdev == NULL) {
		err = -EINVAL;
		goto out;
	}

	fdev = flash_by_bdev(bdev);
	if (fdev == NULL) {
		err = -EINVAL;
		goto out;
	}

	for (i = 0; i < flash_get_blocks_num(fdev); i++) {
		fdev->drv->flash_erase_block(fdev, i);
	}

out:
	if (err) {
		flash_erase_print_usage();
	}

	return err;
}
