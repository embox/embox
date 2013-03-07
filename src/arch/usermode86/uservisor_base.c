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

int emvisor_sendirq(host_pid_t pid, int fd, enum emvisor_msg type,
		const void *msg_data, int dlen) {
	int ret;

	if (0 > (ret = emvisor_send(fd, type, msg_data, dlen))) {
		return ret;
	}

	host_kill(pid, UV_IRQ);

	return 0;
}

int emvisor_recvmsg(int fd, struct emvisor_msghdr *msg) {
	int ret;
	if (0 > (ret = host_read(fd, msg, sizeof(struct emvisor_msghdr)))) {
		return -EIO;
	}

	return 0;
}

int emvisor_recvbody(int fd, const struct emvisor_msghdr *msg, void *data, int dlen) {
	int ldata = msg->dlen;
	void *pb = data;
	int ret;

	if (msg->dlen >= dlen) {
		return -ERANGE;
	}

	while (ldata) {
		if (0 > (ret = host_read(fd, data, ldata))) {
			return -EIO;
		}

		ldata -= ret;
		pb += ret;
	}

	return msg->dlen;
}

int emvisor_recv(int fd, struct emvisor_msghdr *msg, void *data, int dlen) {
	int ret;

	if (0 > (ret = emvisor_recvmsg(fd, msg))) {
		return ret;
	}

	return emvisor_recvbody(fd, msg, data, dlen);

}


