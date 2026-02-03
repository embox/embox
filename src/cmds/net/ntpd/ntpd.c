/**
 * @file
 * @brief NTP Daemon
 *
 * @date 13.07.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <kernel/time/sys_timer.h>

#include <net/lib/ntp.h>

struct ntpd_param {
	int running;
	int sock;
	in_addr_t addr;
	int poll;
	struct sys_timer *tmr;
	int replied;
};

static int send_request(struct ntpd_param *param) {
	int ret;
	struct timespec ts;
	struct ntphdr req;
	struct ntp_data_l xmit_time;

	assert(param != NULL);

	clock_gettime(CLOCK_REALTIME, &ts);
	ret = ntp_timespec_to_data_l(&ts, &xmit_time);
	if (ret != 0) {
		return ret;
	}
	ret = ntp_build(&req, NTP_LI_NO_WARNING, NTP_MOD_CLIENT, 0, 0,
			0, NULL, NULL, NULL, &xmit_time);
	if (ret != 0) {
		return ret;
	}

	if (-1 == send(param->sock, &req, sizeof req, 0)) {
		perror("ntpd: send() failure");
		return -errno;
	}

	return 0;
}

static void timer_handler(struct sys_timer *timer, void *param) {
	int ret;
	struct ntpd_param *param_;

	assert(timer != NULL);
	assert(param != NULL);

	param_ = param;

	if (!param_->replied && param_->poll < NTP_POLL_MAX) {
		++param_->poll;
		sys_timer_close(param_->tmr);
		sys_timer_set(&param_->tmr, SYS_TIMER_PERIODIC,
				(1 << param_->poll) * MSEC_PER_SEC,
				timer_handler, param);
	}

	param_->replied = 0;
	ret = send_request(param_);
	if (ret != 0) {
		printf("timer_handler: error: send_request return %d\n", ret);
	}
}

static int make_socket(int *out_sock, in_addr_t server) {
	int ret;
	struct sockaddr_in addr;

	assert(out_sock != NULL);

	ret = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ret == -1) {
		perror("ntpd: socket() failure");
		return -errno;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(NTP_PORT);
	addr.sin_addr.s_addr = server;

	if (-1 == connect(ret, (struct sockaddr *)&addr,
				sizeof addr)) {
		close(ret);
		perror("ntpd: connect() failure");
		return -errno;
	}

	*out_sock = ret;

	return 0;
}

static int serve(struct ntpd_param *param) {
	int ret;
	struct ntphdr rep;
	struct timespec ts;

	assert(param != NULL);

	while (param->running) {
		ret = recv(param->sock, &rep, sizeof rep, 0);
		if (ret == -1) {
			perror("ntpd: recv() failure");
			return -errno;
		}
		else if (ret < sizeof rep) {
			continue;
		}
		else if (ntp_mode_client(&rep)
				|| !ntp_valid_stratum(&rep)) {
			continue;
		}

		param->replied = 1;
		param->poll = rep.poll;

		ret = ntp_time(&rep, &ts);
		if (ret != 0) {
			return ret;
		}
		clock_settime(CLOCK_REALTIME, &ts);
	}

	return 0;
}

static int ntpd_start(struct ntpd_param *param) {
	int ret;

	if (param->running) {
		return 0;
	}

	ret = make_socket(&param->sock, param->addr);
	if (ret != 0) {
		return ret;
	}

	ret = sys_timer_set(&param->tmr, SYS_TIMER_PERIODIC,
			(1 << param->poll) * MSEC_PER_SEC, timer_handler, param);
	if (ret != 0) {
		close(param->sock);
		return ret;
	}

	param->running = 1;

	return serve(param);
}

static int ntpd_stop(struct ntpd_param *param) {
	assert(param != NULL);

	if (param->running) {
		sys_timer_close(param->tmr);
		close(param->sock);
		param->running = 0;
	}

	return 0;
}

int main(int argc, char **argv) {
	static struct ntpd_param param = {0};
	int opt;

	while (-1 != (opt = getopt(argc, argv, "hS"))) {
		switch(opt) {
		default:
			return -EINVAL;
		case 'h':
			printf("Usage: %s [-Sh] <server-ip>", argv[0]);
			return 0;
		case 'S':
			return ntpd_stop(&param);
		}
	}

	if (optind >= argc) {
		printf("%s: error: no server specified\n", argv[0]);
		return -EINVAL;
	}
	else if (!inet_aton(argv[optind],
				(struct in_addr *)&param.addr)) {
		printf("%s: error: invalid address '%s`\n",
				argv[0], argv[optind]);
		return -EINVAL;
	}

	param.poll = NTP_POLL_MIN;

	return ntpd_start(&param);
}
