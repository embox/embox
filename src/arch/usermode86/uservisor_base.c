/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <errno.h>
#include <unistd.h>
#include <kernel/host.h>
#include <uservisor_base.h>

int emvisor_send(int fd, enum emvisor_msg type, const void *msg_data, int dlen) {
	struct emvisor_msghdr msg = {
		.type = type,
		.dlen = dlen,
	};
	int ret;

	if (0 >= (ret = host_write(fd, &msg, sizeof(struct emvisor_msghdr)))) {
		return ret;
	}

	if (0 >= (ret = host_write(fd, msg_data, dlen))) {
		return ret;
	}

	return 0;
}

int emvisor_sendirq(host_pid_t pid, char signal, int fd, enum emvisor_msg type,
		const void *msg_data, int dlen) {
	int ret;

	if (0 > (ret = emvisor_send(fd, type, msg_data, dlen))) {
		return ret;
	}

	if (signal) {
		host_kill(pid, UV_IRQ);
	}

	return 0;
}

int emvisor_recvmsg(int fd, struct emvisor_msghdr *msg) {
	return host_read(fd, msg, sizeof(struct emvisor_msghdr));
}

int emvisor_recvnbody(int fd, void *data, int dlen) {
	int ldata = dlen;
	void *pb = data;
	int ret;

	while (ldata) {
		if (0 > (ret = host_read(fd, data, ldata))) {
			return -EIO;
		}

		ldata -= ret;
		pb += ret;
	}

	return dlen;
}

int emvisor_recvbody(int fd, const struct emvisor_msghdr *msg, void *data, int dlen) {
	if (msg->dlen > dlen) {
		return -ERANGE;
	}

	return emvisor_recvnbody(fd, data, msg->dlen);
}

int emvisor_recv(int fd, struct emvisor_msghdr *msg, void *data, int dlen) {
	int ret;

	if (0 > (ret = emvisor_recvmsg(fd, msg))) {
		return ret;
	}

	return emvisor_recvbody(fd, msg, data, dlen);

}


