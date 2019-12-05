/**
 * @file
 *
 * @date Dec 4, 2019
 * @date Anton Bondarev
 */
#include <stdio.h>
#include <inttypes.h>

#include <drivers/block_dev.h>

int main(int argc, char **argv) {
	int i;
	struct block_dev **bdevs;

	bdevs = get_bdev_tab();
	assert(bdevs);

	printf("Block devices:\n");
	printf("  ID |  NAME          |    SIZE    | TYPE\n");

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevs[i] && !block_dev_parent(bdevs[i])) {
			int j;
			dev_t id;
			const char *name;
			uint64_t size;

			name = block_dev_name(bdevs[i]);
			size = block_dev_size(bdevs[i]);
			id = block_dev_id(bdevs[i]);

			printf("%4d | %6s         | %10"PRId64" | %s\n",
					id, name, size, "disk");
			for (j = 0; j < MAX_BDEV_QUANTITY; j++) {
				if (bdevs[j] && (bdevs[i] == block_dev_parent(bdevs[j]))) {
					name = block_dev_name(bdevs[j]);
					size = block_dev_size(bdevs[j]);
					id = block_dev_id(bdevs[j]);

					printf("%4d |      |--%6s | %10"PRId64" | %s\n",
							id, name, size, "part");
				}
			}
		}
	}
	printf("\n");

	return 0;
}
