/**
 * @file
 * @brief Configure network interfaces
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-01-19
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SZ 64

void print_usage() {
	printf("USAGE: netmanager [iface]\n");
}

int main(int argc, char **argv) {
	char buf[BUFF_SZ] = "bootpc ";

	if (argc != 2) {
		print_usage();
		return -EINVAL;
	}

	strcat(buf, argv[1]);

	return system(buf);
}
