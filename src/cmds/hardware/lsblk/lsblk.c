/**
 * @file
 *
 * @date Dec 4, 2019
 * @date Anton Bondarev
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <getopt.h>
#include <assert.h>

#include <drivers/block_dev.h>

static const char *convert_unit(uint64_t *size) {
	const char *unit;
	if (*size < 1024) {
		unit = "B";
	}
	else if (*size < 1024 * 1024) {
		unit = "KB";
		*size = *size/1024.0;
	}
	else if (*size < 1024ull * 1024 * 1024) {
		unit = "MB";
		*size = *size/(1024.0 * 1024);
	}
	else if (*size < 1024ull * 1024 * 1024 * 1024) {
		unit = "GB";
		*size = *size/(1024.0 * 1024 * 1024);
	} else {
		unit = "TB";
		*size = *size/(1024.0 * 1024 * 1024 * 1024);
	}
	return unit;
}

int main(int argc, char **argv) {
	int i;
	struct block_dev **bdevs;

	bdevs = get_bdev_tab();
	assert(bdevs);

	int c;
    bool is_bytes = false;

	while ((c = getopt(argc, argv, "b")) != -1) {
        switch (c) {
            case 'b':
                is_bytes = true;
                break;
            default:
                break;
        }
    }

	printf("Block devices:\n");
	printf("  ID |  NAME          |    SIZE    | TYPE\n");

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevs[i] && !block_dev_parent(bdevs[i])) {
			int j;
			dev_t id;
			const char *name;
			const char *unit;
			uint64_t size;

			name = block_dev_name(bdevs[i]);
			size = block_dev_size(bdevs[i]);
			id = block_dev_id(bdevs[i]);

			if (is_bytes) {
				printf("%4d | %6s         | %10" PRId64 " | %s\n",
					   (int)id, name, size, "disk");
			} else {
				unit = convert_unit(&size);
				printf("%4d | %6s         | %8" PRId64 "%s | %s\n",
					   (int)id, name, size, unit, "disk");
			}

			for (j = 0; j < MAX_BDEV_QUANTITY; j++) {
				if (bdevs[j] && (bdevs[i] == block_dev_parent(bdevs[j]))) {
					name = block_dev_name(bdevs[j]);
					size = block_dev_size(bdevs[j]);
					id = block_dev_id(bdevs[j]);

					if (is_bytes) {
						printf("%4d |      |--%6s | %10" PRId64 " | %s\n",
							   (int)id, name, size, "part");
					} else {
						unit = convert_unit(&size);
						printf("%4d | %6s         | %10" PRId64 "%s | %s\n",
							   (int)id, name, size, unit, "disk");
					}
				}
			}
		}
	}
	printf("\n");

	return 0;
}
