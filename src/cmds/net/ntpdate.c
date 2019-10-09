/**
 * @file
 * @brief NTP client
 *
 * @date 13.07.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <kernel/time/time.h>
#include <net/lib/ntp.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <time.h>

#define MODOPS_TIMEOUT OPTION_GET(NUMBER, timeout)

static int make_socket(const struct timeval *timeout, int *out_sock,
		const struct sockaddr_in *addr_in) {
	int ret, sock;

	assert(timeout != NULL);
	assert(out_sock != NULL);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1) {
		ret = -errno;
		assert(ret != 0);
		return ret;
	}

	if (-1 == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
				timeout, sizeof *timeout)) {
		ret = -errno;
		close(sock);
		assert(ret != 0);
		return ret;
	}

	if (-1 == connect(sock, (const struct sockaddr *)addr_in,
				sizeof *addr_in)) {
		ret = -errno;
		close(sock);
		assert(ret != 0);
		return ret;
	}

	*out_sock = sock;

	return 0;
}

static int ntpdate_exec(struct ntphdr *req, in_addr_t server_addr,
		const struct timeval *timeout, struct ntphdr *out_rep) {
	int ret, sock = 0;
	struct sockaddr_in addr;

	assert(req != NULL);
	assert(timeout != NULL);
	assert(out_rep != NULL);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(NTP_PORT);
	addr.sin_addr.s_addr = server_addr;

	ret = make_socket(timeout, &sock, &addr);
	if (ret != 0) {
		return ret;
	}

	if (-1 == send(sock, req, sizeof *req, 0)) {
		ret = -errno;
		goto error;
	}

	do {
		ret = recv(sock, out_rep, sizeof *out_rep, 0);
		if (ret == -1) {
			ret = -errno;
			goto error;
		}
	} while ((ret < sizeof *out_rep)
			|| ntp_mode_client(out_rep));

	ret = 0;

error:
	close(sock);
	return ret;
}

static int ntpdate_process(const struct ntphdr *rep, in_addr_t addr, int only_query) {
	int ret;
	struct timespec ts;

	assert(rep != NULL);

	if (!ntp_valid_stratum(rep)) {
		printf("ntpdate_process: error: invalid stratum (%s)\n",
				ntp_stratum_error(rep));
		return 0; /* error: incorrect packet */
	}

	if (only_query) {
		/* show result */
		struct timespec offset, delay;

		getnsofday(&ts, NULL);

		ret = ntp_offset(rep, &ts, &offset);
		if (ret != 0) {
			return ret;
		}

		ret = ntp_delay(rep, &ts, &delay);
		if (ret != 0) {
			return ret;
		}

		printf("server %s, stratum %hhd, offset %ld.%.6ld, delay %ld.%.6ld\n",
				inet_ntoa(*(struct in_addr *)&addr),
				rep->stratum,
				offset.tv_sec, offset.tv_nsec / NSEC_PER_USEC,
				delay.tv_sec, delay.tv_nsec / NSEC_PER_USEC);

		printf("[%s] adjust time server %s offset %ld.%.6ld sec\n",
				ctime(&ts.tv_sec),
				inet_ntoa(*(struct in_addr *)&addr),
				offset.tv_sec, offset.tv_nsec / NSEC_PER_USEC);
	}
	else {
		/* setup new time */
		ret = ntp_time(rep, &ts);
		if (ret != 0) {
			return ret;
		}
		setnsofday(&ts, NULL);
	}

	return 0;
}

static int ntpdate(in_addr_t addr, const struct timeval *timeout,
		int only_query) {
	int ret;
	struct timespec ts;
	struct ntphdr req, rep;
	struct ntp_data_l xmit_time;

	getnsofday(&ts, NULL);
	ret = ntp_timespec_to_data_l(&ts, &xmit_time);
	if (ret != 0) {
		return ret;
	}
	ret = ntp_build(&req, NTP_LI_NO_WARNING, NTP_MOD_CLIENT, 0, 0,
			0, NULL, NULL, NULL, &xmit_time);
	if (ret != 0) {
		return ret;
	}

	ret = ntpdate_exec(&req, addr, timeout, &rep);
	if (ret != 0) {
		return ret;
	}

	return ntpdate_process(&rep, addr, only_query);
}

int main(int argc, char **argv) {
	int opt, only_query, timeout;
	struct timeval timeout_tv;
	in_addr_t addr;

	only_query = 0;
	timeout = MODOPS_TIMEOUT;
	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hqt:"))) {
		switch (opt) {
		default:
			return -EINVAL;
		case 'h':
			printf("Usage: %s [-q] server", argv[0]);
			return 0;
		case 'q': /* Query only - don't set the clock */
			only_query = 1;
			break;
		case 't': /* Maximum time waiting for a server response */
			if (1 != sscanf(optarg, "%d", &timeout)) {
				printf("%s: error: wrong -t argument %s\n",
						argv[0], optarg);
				return -EINVAL;
			}
			break;
		}
	}

	if (optind >= argc) {
		printf("%s: error: no server specified\n", argv[0]);
		return -EINVAL;
	}
	else if (!inet_aton(argv[optind], (struct in_addr *)&addr)) {
		printf("%s: error: invalid address %s\n", argv[0],
				argv[optind]);
		return -EINVAL;
	}

	timeout_tv.tv_sec = timeout / MSEC_PER_SEC;
	timeout_tv.tv_usec = (timeout % MSEC_PER_SEC) * USEC_PER_MSEC;

	return ntpdate(addr, &timeout_tv, only_query);
}
