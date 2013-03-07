/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#include <errno.h>
#include <kernel/host.h>

int emvisor_send(int fd, enum emvisor_msg type, void *msg_data, int dlen) {
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

int emvisor_recv(int fd, struct emvisor_msghdr *msg, void *data, int dlen) {
	void *pb;
	int ret, ldata;

	if (0 > (ret = host_read(fd, msg, sizeof(struct emvisor_msghdr)))) {
		return -EIO;
	}

	if (msg->dlen >= dlen) {
		return -ERANGE;
	}

	ldata = msg->dlen;
	pb = data;

	while (ldata) {
		if (0 > (ret = host_read(fd, data, ldata))) {
			return -EIO;
		}

		ldata -= ret;
		pb += ret;
	}

	return msg->dlen;
}

static int syscall(int eax, int ebx, int ecx, int edx, int esx, int edi) {
	int ret;
	__asm__ __volatile__(
		"int $0x80\n\t"
		: "=a"(ret)
		: "a"(eax), "b"(ebx), "c"(ecx), "d"(edx), "S"(esx), "D"(edi)
		:
	);

	return ret;
}

int host_read(int fd, void *buf, int len) {
	return syscall(NR_READ, fd, (int) buf, len, 0, 0);
}

int host_write(int fd, const void *buf, int len) {
	return syscall(NR_WRITE, fd, (int) buf, len, 0, 0);
}

int host_kill(host_pid_t pid, int signal) {
	return syscall(NR_KILL, pid, signal, 0, 0, 0);
}

int host_signal(int signum, host_sighandler_t handler) {
	return syscall(NR_SIGNAL, signum, (int) handler, 0, 0, 0);
}

int host_pipe(int *pipe) {
	return syscall(NR_PIPE, (int) pipe, 0, 0, 0, 0);
}

void host_exit(int ret) {
	syscall(NR_EXIT, ret, 0, 0, 0, 0);
}

host_pid_t host_fork(void) {
	char pt_regs[128];
	return syscall(NR_FORK, (int) &pt_regs, 0, 0, 0, 0);
}

int host_close(int fd) {
	return syscall(NR_CLOSE, fd, 0, 0, 0, 0);
}

int host_select(int nfds, host_fd_set *readfds, host_fd_set *writefds,
		host_fd_set *exceptfds, struct host_timeval *timeout) {
	return syscall(NR_SELECT, nfds, (int) readfds, (int) writefds,
			(int) exceptfds, (int) timeout);
}

