/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.07.2013
 */

#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <fs/vfs.h>
#include <fs/node.h>
#include <drivers/ide.h>
#include <drivers/block_dev.h>

#define BSIZE 512

extern block_dev_driver_t *bdev_driver_part;

static struct block_dev *partbdev;
static struct partition part;

int main(int argc, char *argv[]) {
	int opt;
	const char *blockname = NULL;
	const char *partname  = NULL;
	int blockoffset = -1;
	int blocklen = -1;
	node_t *bdevnode = NULL;
	node_t *root;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "b:n:o:l:"))) {
		switch(opt) {
		case 'b':
			blockname = optarg;
			break;
		case 'n':
			partname = optarg;
			break;
		case 'o':
			blockoffset = atoi(optarg);
			break;
		case 'l':
			blocklen = atoi(optarg);
			break;
		default:
			break;
		}
	}

	if (!blockname || !partname || blockoffset < 0 || blocklen < 0) {
		return -1;
	}

	root = vfs_get_root();
	assert(root);
	if (NULL == (bdevnode = vfs_subtree_lookup(root, blockname))) {
		printf("blockdev not found\n");
		return -1;
	}

	part.bdev = bdevnode->nas->fi->privdata;
	part.start = blockoffset;
	part.len = blocklen;

	partbdev = block_dev_create("/dev/hda0", bdev_driver_part, &part);
	partbdev->size = BSIZE * blocklen;

	return 0;

}
