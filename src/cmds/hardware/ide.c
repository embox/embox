/**
 * @file
 * @brief
 *
 * @date 20.07.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include <drivers/ide.h>
#include <drivers/block_dev.h>
#include <fs/inode.h>

static void print_usage(void) {
	printf("Usage: ide \n");
}

static void print_drive (struct ide_tab *ide) {
	hd_t *drive;
	uint64_t dev_size;
	size_t dev_bsize;

	for(int i  = 0; i < 4; i++) {
		printf("\nIDE Channel %d-%d: ", i/2, i%2);

		if (i%2) {
			printf(" Slave  Disk:");
		} else {
			printf(" Master Disk:");
		}
		if (NULL == ide->drive[i]) {
			printf(" None");
		} else {
			drive = (hd_t *) ide->drive[i];
			assert(drive);
			dev_bsize = block_dev_block_size(drive->bdev);
			dev_size = block_dev_size(drive->bdev);
			printf(" %s;", block_dev_name(drive->bdev));
			printf(" %s", drive->param.serial);
			printf(" %s", drive->param.model);
			printf(" %5.3fM", ((float) dev_size) * dev_bsize / (1024 * 1024));
		}
	}
	printf("\n");

}

int main(int argc, char **argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "ah"))) {
		switch(opt) {
		case 'a':
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return -EINVAL;
		}
	}

	print_drive(ide_get_drive());
	return 0;
}
