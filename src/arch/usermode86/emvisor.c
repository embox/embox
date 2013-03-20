/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <uservisor_base.h>

#define DATA_BUFLEN 64
#define SELECT_TO_SEC 5

extern void kernel_start(void);

static struct npipe {
	int np_read;
	int np_write;
} ev_downstream, ev_upstream;

static pid_t emboxpid = -1;
static char need_signal = 1;

static int sendirq(host_pid_t pid, int fd, enum emvisor_msg type,
		const void *data, int dlen) {
	int sg = need_signal;
	int ret;

	if (need_signal) {
		need_signal = 0;
	}

	ret = emvisor_sendirq(pid, sg, fd, type, data, dlen);
	if (-1 == ret) {
		ret = -errno;
	}

	if (ret == -EAGAIN) {
		fprintf(stderr, "Warning: downstream is full\n");
		ret = 0;
	}

	return ret;

}

static int maxfd;
typedef int (*fdact_t)(int fd);
static fdact_t fdact[FD_SETSIZE];
static fd_set refrfds;

static int fdact_add(int fd, fdact_t act) {
	if (FD_ISSET(fd, &refrfds)) {
		return -EBUSY;
	}

	maxfd = fd > maxfd ? fd : maxfd;

	FD_SET(fd, &refrfds);
	fdact[fd] = act;

	return 0;
}

static void fdact_del(int fd) {
	FD_CLR(fd, &refrfds);
	/*not updating maxfd is not fatal or even error,
	 *just select will perform slower, than it could be
	 */
}

static int act_tmr(int tmr) {
	unsigned long long ovrn_count;
	int ret;

	if (0 > (ret = read(tmr, &ovrn_count, sizeof(ovrn_count)))) {
		return ret;
	}

	if (ovrn_count > 1000) {
		fprintf(stdout, "ovrn is %lld\n", ovrn_count);
	}

	if (0 > (ret = sendirq(emboxpid, ev_downstream.np_write,
			EMVISOR_IRQ_TMR, &ovrn_count, sizeof(ovrn_count)))) {
		return ret;
	}

	return 0;
}

static int tmrset(int pupstream, const struct emvisor_msghdr *msg) {
	struct emvisor_tmrset ts;
	int tfd;
	unsigned long long nsec;
	int ret;

	if (0 > (ret = emvisor_recvbody(pupstream, msg,
					&ts, sizeof(ts)))) {
		return ret;
	}

	if (0 > (tfd = timerfd_create(CLOCK_REALTIME, 0))) {
		return tfd;
	}

	if (0 > (ret = fdact_add(tfd, act_tmr))) {
		return ret;
	}

	nsec = 1000000000 / ts.overfl_fq;

	{
		struct itimerspec timespec = {
			.it_interval = {
				.tv_sec = 0,
				.tv_nsec = nsec,
			},
			.it_value = {
				.tv_sec = 0,
				.tv_nsec = nsec,
			},
		};

		if (0 > (ret = timerfd_settime(tfd, 0, &timespec, NULL))) {
			return ret;
		}
	}

	return 0;
}

static int upstrm_cmd(int pupstream);

static int act_upstrrd(int pupstream) {
	int ret;

	while (0 < (ret = upstrm_cmd(pupstream))) {

	}

	if (ret == -EAGAIN) {
		ret = 0;
	}

	return ret;
}

static int upstrm_cmd(int pupstream) {
	struct emvisor_msghdr msg;
	char buf[DATA_BUFLEN];
	int flags, ret;

	assert(pupstream == ev_upstream.np_read);

	if (0 >= (ret = emvisor_recvmsg(pupstream, &msg))) {
		return ret;
	}

	/*fprintf(stderr, "msg: %d, %d\n", msg.type, msg.dlen);*/

	assert(ret == sizeof(struct emvisor_msghdr));

	if (!ret) {
		return -EPIPE;
	}

	assert(msg.type < EMVISOR_IRQ);
	assert(msg.dlen <= 8);

	switch (msg.type) {
	case EMVISOR_BUDDY_PID:
		if (0 > (ret = emvisor_recvbody(pupstream, &msg,
						&emboxpid, sizeof(emboxpid)))) {
			return ret;
		}

		fprintf(stderr, "New embox pid is %d\n", emboxpid);
		return 0;
	case EMVISOR_DIAG_OUT:
		if (0 > (ret = emvisor_recvbody(pupstream, &msg,
						buf, DATA_BUFLEN))) {
			return ret;
		}

		if (0 > (ret = write(2, buf, msg.dlen))) {
			return ret;
		}
		return 0;

	case EMVISOR_DIAG_IN:
		if (0 > (ret = emvisor_recvbody(pupstream, &msg,
						buf, DATA_BUFLEN))) {
			return ret;
		}

		ret = read(0, buf, 1);

		return sendirq(emboxpid, ev_downstream.np_write,
				EMVISOR_IRQ_DIAG_IN, buf, ret < 0 ? 0 : ret);

	case EMVISOR_EOF_IRQ:

		/*fprintf(stdout, "got irq ack\n");*/
		need_signal = 1;

		return 0;
	case EMVISOR_TIMER_SET:
		return tmrset(pupstream, &msg);
	default:
		return -ENOENT;
	}

	return -ENOENT;

}

static int emvisor(void) {
	fd_set rfds;
	struct timeval tv = {SELECT_TO_SEC, 0};
	int pupstream = ev_upstream.np_read;
	int ret, i;

	if (0 > (ret = fdact_add(pupstream, act_upstrrd))) {
		return ret;
	}

	memcpy(&rfds, &refrfds, sizeof(fd_set));

	while (0 <= (ret = select(maxfd + 1, &rfds, NULL, NULL, &tv))) {
		if (ret == 0) {
			goto new_select;
		}

		for (i = 0; i <= maxfd; i++) {
			if (!FD_ISSET(i, &rfds)) {
				continue;
			}

			if (fdact[i]) {
				if (0 > (ret = fdact[i](i))) {
					return ret;
				}
			}

		}

	new_select:
		memcpy(&rfds, &refrfds, sizeof(fd_set));

		tv.tv_sec = SELECT_TO_SEC;
		tv.tv_usec = 0;

	}

	return ret;
}

int main(int argc, char **argv) {
	int fd, flags, ret;

	if (argc != 3) {
		return -EINVAL;
	}

	flags = fcntl(STDIN_FILENO, F_GETFL);
	if (0 > (ret = fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK))) {
		return -errno;
	}

	if (0 > (fd = open(argv[1], O_WRONLY))) {
		return -errno;
	}

	flags = fcntl(fd, F_GETFL);
	if (0 > (ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK))) {
		return -errno;
	}

	ev_downstream.np_write = fd;

	if (0 > (fd = open(argv[2], O_RDONLY))) {
		return -errno;
	}

	flags = fcntl(fd, F_GETFL);
	if (0 > (ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK))){
		return -errno;
	}

	ev_upstream.np_read = fd;

	ret = emvisor();

	printf("Emvisor exit: %d\n", ret);
	return ret;

}

int host_write(int fd, const void *buf, size_t len) {
	int ret = write(fd, buf, len);
	return -1 == ret ? -errno : ret;
}

int host_read(int fd, void *buf, size_t len) {
	int ret = read(fd, buf, len);
	return -1 == ret ? -errno : ret;
}

int host_kill(pid_t pid, int sig) {
	int ret = kill(pid, sig);
	return -1 == ret ? -errno : ret;
}

