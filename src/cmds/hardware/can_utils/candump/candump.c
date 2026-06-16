/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 26.05.26
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <drivers/char_dev.h>

static void print_usage(void) {
	printf("Usage: candump <device>\n\n");
}

static void print_error(const char *func, const char *err_msg) {
	printf("%s(): %s\n", func, err_msg);
}

int main(int argc, char **argv) {
	struct canfd_frame frame;
	int fd;
	int i;

	if (argc != 2) {
		print_usage();
		return 1;
	}

	fd = char_dev_open(argv[1], O_RDWR);
	if (fd < 0) {
		print_error("char_dev_open", strerror(-fd));
		return 1;
	}

	while (1) {
		if (-1 == read(fd, &frame, CANFD_MTU)) {
			print_error("read", strerror(errno));
			break;
		}

		printf("  %s", argv[1]);
		if (frame.can_id & CAN_EFF_FLAG) {
			printf("  %08x", frame.can_id & CAN_EFF_MASK);
		}
		else {
			printf("  %03x", frame.can_id & CAN_SFF_MASK);
		}
		printf("   [%d] ", frame.len);
		for (i = 0; i < frame.len; i++) {
			printf(" %02x", frame.data[i]);
		}
		printf("\n");
	}

	close(fd);

	return 1;
}
