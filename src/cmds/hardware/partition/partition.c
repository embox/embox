/**
 * @file
 *
 * @date Feb 11, 2020
 * @author Anton Bondarev
 */

#include <assert.h>

#include <drivers/block_dev.h>
#include <drivers/block_dev/partition.h>

int main(int argc, char **argv) {
	int i;
	struct block_dev **bdevs;

	bdevs = get_bdev_tab();
	assert(bdevs);

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevs[i] && !block_dev_parent(bdevs[i])) {
			create_partitions(bdevs[i]);
		}
	}

	return 0;
}
