/**
 * @file fpga.c
 * @brief Load config to FPGA
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.01.2020
 */
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <drivers/fpga.h>

static void print_help(char **argv) {
	printf("USAGE:\n");
	printf("%s [load file_name | tftp file_name]\n", argv[0]);
}

#define FPGA_DEFAULT_ID 0
#define BUF_SZ (16 * 1024 * 1024)
static uint8_t buf[BUF_SZ];
int main(int argc, char **argv) {
	struct fpga *fpga;
	struct stat st;
	int fd;
	int err;
	char *fname;

	if (argc != 3) {
		print_help(argv);
		return 0;
	}

	fpga = fpga_by_id(FPGA_DEFAULT_ID);
	if (fpga == NULL) {
		printf("fpga%d not found\n", FPGA_DEFAULT_ID);
		return 0;
	}

	fname = argv[2];
	fd = open(fname, O_RDONLY);
	if (fd < 0) {
		printf("Failed to open %s\n", fname);
		return -1;
	}

	err = fstat(fd, &st);
	if (err < 0) {
		printf("Failed to determine file size, exiting\n");
		return -1;
	} else if (st.st_size > BUF_SZ) {
		printf("File size is to large,"
				" need to be less than %d bytes"
				"(%s is %jd bytes)\n",
				BUF_SZ,
				fname,
				(intmax_t)st.st_size);
		return -1;
	}

	err = read(fd, buf, BUF_SZ);
	if (err == 0) {
		printf("Failed to read %s\n", fname);
		return -1;
	}

	err = close(fd);
	if (err != 0) {
		printf("Warning: failed to close %s after read, "
				"try to write config anyway\n",
				fname);
	}

	err = fpga->ops->config_init(fpga);
	if (err != 0) {
		printf("FPGA config init failed with code %d! Exiting\n",
				err);
		return -1;
	}

	err = fpga->ops->config_write(fpga, buf, st.st_size);
	if (err != 0) {
		printf("FPGA config write failed with code %d! Exiting\n",
				err);
		return -1;
	}

	err = fpga->ops->config_complete(fpga);
	if (err != 0) {
		printf("FPGA config finish failed with code %d!\n",
				err);
		return -1;
	}

	return 0;
}
