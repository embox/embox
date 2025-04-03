#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <drivers/gpio.h>
#include <drivers/serial/uart_dev.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

static void print_usage(const char *cmd) {
	printf("Usage: %s [-h] /dev/ttyS<0/1>\n", cmd);
}

int main(int argc, char **argv) {
	int fd;
	char * msg = "Test message!";
	char buff[40];

	if (argc < 2) {
		print_usage(argv[0]);
		return 0;
	}
	for (int opt; -1 != (opt = getopt(argc, argv, "h"));) {
		switch (opt) {
		case 'h':
		default:
			print_usage(argv[0]);
			return 0;
		}
	}
	
	printf("%s in/out test\n", argv[argc - 1]);
	fflush(stdout);
	
	fd = open(argv[argc - 1], O_RDWR);
	if (fd==-1) {
		printf("Failed to open %s\n", argv[argc - 1]);
		return -1;
	}

	printf("Writing: \"%s\"...", msg);
	if (write(fd, msg, strlen(msg)) != strlen(msg)) {
		printf("Failed to write %s", msg);
		return -1;
	}
	write(fd, "\n", 1); //	fsync(fd);
	printf("OK\n");
/*
	if (read(fd, buff, strlen(msg)) != strlen(msg)) {
		printf("Failed to read %s", buff);
		return -1;
	}

	printf("Read buff: %s\n", buff);
*/

	printf("Reading: ");
	for (int i=0; i<strlen(msg); i++) {
		read(fd, buff+i, 1);
		printf("%c", buff[i]);
		fflush(stdout);
	}
	printf("\nOK\n");

	close(fd);

	return 0;
}
