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
#include <termios.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include "include/uservisor_base.h"

#include <net/if.h>
#include <linux/if_tun.h>
#include <linux/ioctl.h>

#define DATA_BUFLEN 64
#define SELECT_TO_SEC 5

extern void kernel_start(void);

static struct npipe {
	int np_read;
	int np_write;
} ev_downstream, ev_upstream;

static pid_t emboxpid = -1;
static char need_signal = 1;

static int sirq(host_pid_t pid) {
	int sg = need_signal;

	need_signal = 0;

	return emvisor_irq(pid, sg);
}

static int sendirq(host_pid_t pid, int fd, enum emvisor_msg type,
		const void *data, int dlen) {
	int sg = need_signal;
	int ret;

	need_signal = 0;

	ret = emvisor_sendirq(pid, sg, fd, type, data, dlen);
	if (-1 == ret) {
		ret = -errno;
	}

	if (ret == -EAGAIN) {
		/*fprintf(stderr, "Warning: downstream is full\n");*/
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

static char tapnam[16] = "tap77";

static struct {
	struct emvisor_netstate state;
	char *tapname;
	int fd;
} devstate = {
	.state = {
		.id = -1,
	},
	.tapname = tapnam,
	.fd = -1,
};

static int devstate_nextid;

static int tun_alloc(char *dev) {
	struct ifreq ifr;
	int fd, err;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		return -errno;
	}

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	*        IFF_TAP   - TAP device
	*
	*        IFF_NO_PI - Do not provide packet information
	*/
	ifr.ifr_flags = IFF_TAP;
	if (*dev)
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
		close(fd);
		return -errno;
	}

	strcpy(dev, ifr.ifr_name);
	return fd;
}

#define NETDATA_BUFLEN 1518
static char netdata_buf[NETDATA_BUFLEN];

static int netdata_act(int tapfd) {
	int len = read(tapfd, netdata_buf, NETDATA_BUFLEN) - 4;
	char *data = netdata_buf + 4;
	struct emvisor_netdata_hdr dhdr = {
		.dev_id = devstate.state.id,
		.len = len,
	};
	int ret;

	if (!(devstate.state.mode == NETSTATE_MODON)) {
		return 0;
	}

	if (0 > (ret = emvisor_sendhdr(ev_downstream.np_write,
				       EMVISOR_IRQ_NETDATA, sizeof(dhdr) + len))) {
		return ret;
	}

	if (0 > (ret = emvisor_sendn(ev_downstream.np_write, &dhdr,
					sizeof(dhdr)))) {
		return ret;
	}

	if (0 > (ret = emvisor_sendn(ev_downstream.np_write, data, len))) {
		return ret;
	}

	sirq(emboxpid);

	return len;
}

static int netctl(int pup, const struct emvisor_msghdr *msg) {
	struct emvisor_netstate state;
	int fd;
	int res;

	if (0 > (res = emvisor_recvbody(pup, msg, &state, sizeof(state)))) {
		return res;
	}

	if (state.id == NETSTATE_REQUEST) {
		if (devstate.state.id >= 0)  {
			struct emvisor_netstate ack = {
				.id = NETSTATE_NODEV,
			};

			if (0 > (res = emvisor_send(ev_downstream.np_write,
					EMVISOR_IRQ_NETCTL, &ack, sizeof(ack)))) {
				return res;
			}

			return 0;
		}

		devstate.state.id = devstate_nextid ++;

		if (0 > (res = emvisor_send(ev_downstream.np_write,
				EMVISOR_IRQ_NETCTL, &devstate.state,
				sizeof(devstate.state)))) {
			return res;
		}

		return 0;
	}

	if (state.id != devstate.state.id) {
		fprintf(stderr, "netctl: on %d with %d\n", state.id,
				devstate.state.id);
		return -ENODEV;
	}

	memcpy(&devstate.state, &state, sizeof(devstate.state));

	if (devstate.fd >= 0) {
		return 0;
	}

	if (0 <= (devstate.fd = tun_alloc(devstate.tapname))) {
		fdact_add(devstate.fd, netdata_act);
	} else {
		fprintf(stderr, "requested not existing tap\n");
	}

	return 0;

}

static int netdata(int pup, const struct emvisor_msghdr *msg) {
	struct emvisor_netdata_hdr dhdr;
	int fd;
	int res;

	if (0 > (res = emvisor_recvnbody(pup, &dhdr, sizeof(dhdr)))) {
		return res;
	}

	if (devstate.state.id != dhdr.dev_id) {
		return -ENODEV;
	}

	if (dhdr.len >= NETDATA_BUFLEN) {
		return -ERANGE;
	}

	if (0 > (res = emvisor_recvnbody(pup, netdata_buf + 4, dhdr.len))) {
		return res;
	}

	if (devstate.fd < 0) {
		return 0;
	}

	memset(netdata_buf, 0, 4);

	return write(devstate.fd, netdata_buf, dhdr.len + 4);
}

#define MAX_STDIN_RD_REQ 64
static int act_stdin_rd_req, act_stdin_rd_req_len;
char act_stdin_rd_buf[MAX_STDIN_RD_REQ], *act_stdin_rd_pb;

static int act_stdin_rd(int pupstream) {
	int ret;

	ret = read(STDIN_FILENO, act_stdin_rd_pb, act_stdin_rd_req);

	if (ret == -1) {
		ret = -errno;
	}

	if (ret == 0 || ret == -EAGAIN) {
		return 0;
	}

	if (ret < 0) {
		return ret;
	}

	act_stdin_rd_req -= ret;
	act_stdin_rd_pb += ret;

	if (act_stdin_rd_req) {
		return 0;
	}

	if (0 > (ret = sendirq(emboxpid, ev_downstream.np_write,
					EMVISOR_IRQ_DIAG_IN, act_stdin_rd_buf,
					act_stdin_rd_req_len))) {
		return ret;
	}

	fdact_del(STDIN_FILENO);

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
	assert(msg.dlen <= 1600);

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

		if (0 != (ret = fdact_add(STDIN_FILENO, act_stdin_rd))) {
			return ret;
		}

		act_stdin_rd_pb = act_stdin_rd_buf;
		act_stdin_rd_req = act_stdin_rd_req_len = 1;

		return 0;

	case EMVISOR_EOF_IRQ:

		/*fprintf(stderr, "!");*/
		need_signal = 1;

		return 0;
	case EMVISOR_TIMER_SET:
		return tmrset(pupstream, &msg);
	case EMVISOR_NETCTL:
		return netctl(pupstream, &msg);
	case EMVISOR_NETDATA:
		return netdata(pupstream, &msg);
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
	struct termios tc_old, tc_this;

	if (argc != 3) {
		return -EINVAL;
	}

	if (0 > tcgetattr(STDIN_FILENO, &tc_old)) {
		return -errno;
	}

	memcpy(&tc_this, &tc_old, sizeof(struct termios));

	tc_this.c_lflag &= ~(ICANON | ECHO);

	if (0 > tcsetattr(STDIN_FILENO, TCSANOW, &tc_this)) {
		return -errno;
	}

	if (0 > (fd = open(argv[1], O_WRONLY))) {
		return -errno;
	}

#if 0
	flags = fcntl(fd, F_GETFL);
	if (0 > (ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK))) {
		return -errno;
	}
#endif
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

	tcsetattr(STDIN_FILENO, TCSANOW, &tc_old);

	printf("Emvisor exit: %d\n", ret);
	pause();
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

