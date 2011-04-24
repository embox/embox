/**
 * @file
 * @brief Load image file into memory.
 *
 * @date 03.07.09
 * @author Sergey Kuzmin
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <stdio.h>
#include <string.h>
#include <lib/cpio.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: load [-a addr] [-f filename] [-h]\n");
}

static int exec(int argc, char **argv) {
	char *file_name = NULL;
	uint32_t load_addr, file_addr;
	FILE *file;
	node_t *node;
	stat_t sb;
	int opt;
	getopt_init();
	do {
		opt = getopt(argc, argv, "f:a:h");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'f':
			file_name = optarg;
			break;
		case 'a':
			if ((optarg != NULL) &&
			    (!sscanf(optarg, "0x%x", &load_addr)) &&
			    (!sscanf(optarg, "%d", (int *) &load_addr))) {
				LOG_ERROR("hex value expected.\n");
				print_usage();
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	if (NULL == (file = fopen(file_name, "r"))) {
		printf("Can't open file %s\n", file_name);
		return -1;
	}
	fioctl(file, 0, &file_addr);

	node = vfs_find_node(file_name, NULL);
	fstat(file_name, &sb);

	printf("loading...addr=0x%08x, size=%d\n", file_addr, sb.st_size);
	memcpy((void *) load_addr, (void *) file_addr, sb.st_size);
	fclose(file);
	return 0;
}
