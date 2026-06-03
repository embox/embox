/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 26.05.26
 */

#include <fcntl.h>
#include <linux/can.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
	struct can_frame frame;
	int fd;

	if (argc == 2) {
		fd = open(argv[1], O_RDWR);
	}
	else {
		fd = open("/dev/can0", O_RDWR);
	}

	frame.can_id = 0x123;
	frame.len = 8;
	frame.data[0] = 0x00;
	frame.data[1] = 0x11;
	frame.data[2] = 0x22;
	frame.data[3] = 0x33;
	frame.data[4] = 0x44;
	frame.data[5] = 0x55;
	frame.data[6] = 0x66;
	frame.data[7] = 0x77;

	if (write(fd, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		printf("Write failed\n");
		return -1;
	}

	close(fd);

	return 0;
}
