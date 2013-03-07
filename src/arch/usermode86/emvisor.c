/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <uservisor_base.h>

#define DATA_BUFLEN 64

extern void kernel_start(void);

int host_write(int fd, const void *buf, size_t len) {
	return write(fd, buf, len);
}

int host_read(int fd, void *buf, size_t len) {
	return read(fd, buf, len);
}

int host_kill(pid_t pid, int sig) {
	return kill(pid, sig);
}

static struct npipe {
	int np_read;
	int np_write;
} ev_downstream, ev_upstream;

static int embox(const char *file, int pdownstream, int pwdownstream,
		int pupstream) {

	int ret;

	dup2(pdownstream,  UV_PRDDOWNSTRM);
	dup2(pwdownstream, UV_PWRDOWNSTRM);
	dup2(pupstream,    UV_PWRUPSTRM);

	close(pdownstream);
	close(pwdownstream);
	close(pupstream);

	if (-1 == (ret = execlp(file, file, NULL))) {
		ret = -errno;
	}

	return ret;
}

static int recvd(pid_t emboxpid, int pdownstream, int pupstream) {
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
		if (0 > (ret = write(1, buf, msg.dlen))) {
			return ret;
		}
		return 0;

	case EMVISOR_DIAG_IN:
		if (0 > (ret = read(0, buf, 1))) {
			return ret;
		}

		emvisor_sendirq(emboxpid, pdownstream, EMVISOR_IRQ_DIAG_IN, buf, 1);

		return 0;
	default:
		return -ENOENT;
	}

	return -ENOENT;

}

static int emvisor(pid_t emboxpid, int pdownstream, int pupstream) {
	fd_set rfds;
	struct timeval tv = {1, 0};
	int ret;

	FD_ZERO(&rfds);
	FD_SET(pupstream, &rfds);

	while (0 <= (ret = select(pupstream + 1, &rfds, NULL, NULL, &tv))) {
		if (FD_ISSET(pupstream, &rfds)) {
			if (0 > (ret = recvd(emboxpid, pdownstream, pupstream))) {
			       return ret;
			}
		}

		FD_ZERO(&rfds);
		FD_SET(pupstream, &rfds);

		tv.tv_sec = 10;
		tv.tv_usec = 0;

	}

	return ret;
}

int main(int argc, char **argv) {
	pid_t pid;
	int ret;

	if (argc != 2) {
		return -EINVAL;
	}

	if (0 != pipe((int *) &ev_downstream)) {
		exit(1);
	}

	if (0 != pipe((int *) &ev_upstream)) {
		exit(1);
	}

	if (0 == (pid = fork())) {
		ret = embox(argv[1], ev_downstream.np_read, ev_downstream.np_write,
				ev_upstream.np_write);
	} else {
		ret = emvisor(pid, ev_downstream.np_write, ev_upstream.np_read);
	}

	exit(ret);
}
