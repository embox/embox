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

static struct npipe {
	int np_read;
	int np_write;
} ev_downstream, ev_upstream;

int embox_getwdownstream(void) {
	return ev_downstream.np_write;
}

int embox_getupstream(void) {
	return ev_upstream.np_write;
}

int embox_getdownstream(void) {
	return ev_downstream.np_read;
}

static void embox(int pdownstream, int pupstream) {

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

	case EMVISOR_DIAG_IN:
		if (0 > (ret = host_read(0, buf, 1))) {
			return ret;
		}

		emvisor_sendirq(emboxpid, pdownstream, EMVISOR_IRQ_DIAG_IN, buf, 1);

		return 0;
	default:
		return -ENOENT;
	}

	return -ENOENT;

}

static int emvisor(host_pid_t emboxpid, int pdownstream, int pupstream) {
	host_fd_set rfds;
	struct host_timeval tv = {1, 0};
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
	host_pid_t pid;
	int ret;

	if (0 != host_pipe((int *) &ev_downstream)) {
		host_exit(1);
	}

	if (0 != host_pipe((int *) &ev_upstream)) {
		host_exit(1);
	}

	if (0 == (pid = host_fork())) {
		embox(ev_downstream.np_read, ev_upstream.np_write);
		ret = 0;
	} else {
		ret = emvisor(pid, ev_downstream.np_write, ev_upstream.np_read);
	}

	host_exit(ret);
}
