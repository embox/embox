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

static pid_t emboxpid;
static char need_signal = 1;

static int sendirq(host_pid_t pid, int fd, enum emvisor_msg type,
		const void *data, int dlen) {
	int sg = need_signal;

	if (need_signal) {
		need_signal = 0;
	}

	return emvisor_sendirq(pid, sg, fd, type, data, dlen);
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

static int embox(const char *file) {
	int pdownstream = ev_downstream.np_read;
	int pwdownstream = ev_downstream.np_write;
	int pupstream = ev_upstream.np_write;
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

static int act_tmr(int tmr) {
	unsigned long long ovrn_count;
	int ret;

	if (0 > (ret = read(tmr, &ovrn_count, sizeof(ovrn_count)))) {
		return ret;
	}

	/*fprintf(stdout, "ovrn is %lld\n", ovrn_count);*/

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

static int act_upstrrd(int pupstream) {
	struct emvisor_msghdr msg;
	char buf[DATA_BUFLEN];
	int ret;

	assert(pupstream == ev_upstream.np_read);

	if (0 > (ret = emvisor_recvmsg(pupstream, &msg))) {
		return ret;
	}

	if (!ret) {
		return -EPIPE;
	}

	switch (msg.type) {
	case EMVISOR_DIAG_OUT:
		if (0 > (ret = emvisor_recvbody(pupstream, &msg,
						buf, DATA_BUFLEN))) {
			return ret;
		}

		if (0 > (ret = write(1, buf, msg.dlen))) {
			return ret;
		}
		return 0;

	case EMVISOR_DIAG_IN:
		if (0 > (ret = emvisor_recvbody(pupstream, &msg,
						buf, DATA_BUFLEN))) {
			return ret;
		}

		if (0 > (ret = read(0, buf, 1))) {
			return ret;
		}

		return sendirq(emboxpid, ev_downstream.np_write,
				EMVISOR_IRQ_DIAG_IN, buf, 1);

		return 0;
	case EMVISOR_EOF_IRQ:

		fprintf(stdout, "got irq ack\n");
		need_signal = 1;

		return 0;
	case EMVISOR_TIMER_SET:
		return tmrset(pupstream, &msg);
	default:
		return -ENOENT;
	}

	return -ENOENT;

}

static int emvisor(pid_t emboxpid) {
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
	int fd, ret;

	if (argc != 4) {
		return -EINVAL;
	}

	if (0 >= (sscanf(argv[1], "%d", &emboxpid))) {
		return -EINVAL;
	}


	if (0 > (fd = open(argv[2], O_WRONLY))) {
		return -EINVAL;
	}
	ev_downstream.np_write = fd;

	if (0 > (fd = open(argv[3], O_RDONLY))) {
		return -EINVAL;
	}
	ev_upstream.np_read = fd;

	return emvisor(emboxpid);

}

int host_write(int fd, const void *buf, size_t len) {
	return write(fd, buf, len);
}

int host_read(int fd, void *buf, size_t len) {
	return read(fd, buf, len);
}

int host_kill(pid_t pid, int sig) {
	return kill(pid, sig);
}

