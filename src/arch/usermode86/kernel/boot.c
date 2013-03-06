/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#include <errno.h>
#include <stddef.h>
#include <kernel/host.h>

#define DATA_BUFLEN 64

extern void kernel_start(void);

static int embox_downstream, embox_upstream;

int embox_getupstream(void) {
	return embox_upstream;
}

int embox_getdownstream(void) {
	return embox_downstream;
}

static void embox(int pdownstream, int pupstream) {

	embox_downstream = pdownstream;
	embox_upstream   = pupstream;

	kernel_start();
}

static int recvd(host_pid_t emboxpid, int pdownstream, int pupstream) {
	struct emvisor_msghdr msg;
	char buf[DATA_BUFLEN];
	int ret;

	if (0 > (ret = emvisor_recv(pupstream, &msg, buf, DATA_BUFLEN))) {
		return ret;
	}

	if (!ret) {
		return -EPIPE;
	}

	switch (msg.type) {
	case EMVISOR_DIAG_OUT:
		if (0 > (ret = host_write(1, buf, msg.dlen))) {
			return ret;
		}

		return 0;
	default:
		return -ENOENT;
	}

	return -ENOENT;

}

static int emvisor(host_pid_t emboxpid, int pdownstream, int pupstream) {
	host_fd_set rfds;
	struct host_timeval tv = {10, 0};
	int ret;

	HOST_FD_ZERO(&rfds);
	HOST_FD_SET(pupstream, &rfds);

	while (0 <= (ret = host_select(pupstream + 1, &rfds, NULL, NULL, &tv))) {
		if (HOST_FD_ISSET(pupstream, &rfds)) {
			if (0 > (ret = recvd(emboxpid, pdownstream, pupstream))) {
			       return ret;
			}
		}

		HOST_FD_ZERO(&rfds);
		HOST_FD_SET(pupstream, &rfds);

		tv.tv_sec = 10;
		tv.tv_usec = 0;

	}

	return ret;
}

void _start(void) {
	int pdownstream[2], pupstream[2];
	host_pid_t pid;
	int ret;

	if (0 != host_pipe(pdownstream)) {
		host_exit(1);
	}

	if (0 != host_pipe(pupstream)) {
		host_exit(1);
	}

	if (0 == (pid = host_fork())) {
		host_close(pdownstream[1]);
		host_close(pupstream[0]);
		embox(pdownstream[0], pupstream[1]);
		ret = 0;
	} else {
		host_close(pdownstream[0]);
		host_close(pupstream[1]);
		ret = emvisor(pid, pdownstream[1], pupstream[0]);
	}

	host_exit(ret);
}
