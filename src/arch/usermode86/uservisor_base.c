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

int emvisor_sendhdr(int fd, enum emvisor_msg type, int dlen) {
	struct emvisor_msghdr msg = {
		.type = type,
		.dlen = dlen,
	};

	return host_write(fd, &msg, sizeof(struct emvisor_msghdr));
}

int emvisor_sendn(int fd, const void *data, int len) {
	return host_write(fd, data, len);
}

int emvisor_send(int fd, enum emvisor_msg type, const void *msg_data, int dlen) {
	int ret;

	if (0 >= (ret = emvisor_sendhdr(fd, type, dlen))) {
		return ret;
	}

	return emvisor_sendn(fd, msg_data, dlen);
}

int emvisor_irq(host_pid_t pid, char signal) {
	if (signal) {
		return host_kill(pid, UV_IRQ);
	}

	return 0;
}

int emvisor_sendirq(host_pid_t pid, char signal, int fd, enum emvisor_msg type,
		const void *msg_data, int dlen) {
	int ret;

	if (0 > (ret = emvisor_send(fd, type, msg_data, dlen))) {
		return ret;
	}

	return emvisor_irq(pid, signal);
}

int emvisor_recvn(int fd, void *data, int dlen) {
	int ldata = dlen;
	void *pb = data;
	int ret;

	if (0 >= (ret = host_read(fd, data, ldata))) {
		return ret;
	}

	ldata -= ret;
	pb += ret;

	while (ldata) {
		if (0 > (ret = host_read(fd, pb, ldata))) {
			if (ret == -EAGAIN) {
				continue;
			}
			return ret;
		}

		ldata -= ret;
		pb += ret;
	}

	return dlen;
}

int emvisor_recvmsg(int fd, struct emvisor_msghdr *msg) {
	return emvisor_recvn(fd, msg, sizeof(struct emvisor_msghdr));
}

int emvisor_recvnbody(int fd, void *data, int dlen) {
	int ret;

	while (0 >= (ret = emvisor_recvn(fd, data, dlen))) {
		if (ret && ret != -EAGAIN) {
			break;
		}

	}

	return ret;
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
