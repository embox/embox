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
	struct canfd_frame frame;
	int fd;

	if (argc == 2) {
		fd = open(argv[1], O_RDWR);
	}
	else {
		fd = open("/dev/can0", O_RDWR);
	}

	while (1) {
		if (read(fd, &frame, sizeof(struct canfd_frame))
		    != sizeof(struct canfd_frame)) {
			printf("read failed\n");
			return -1;
		}

		printf("Recv: can_id = 0x%08x, len=%d\n"
		       "data(%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x)\n",
		    frame.can_id, frame.len, frame.data[0], frame.data[1],
		    frame.data[2], frame.data[3], frame.data[4], frame.data[5],
		    frame.data[6], frame.data[7]);
	}

	close(fd);

	return 0;
}
