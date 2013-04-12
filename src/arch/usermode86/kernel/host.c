/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#include <errno.h>
#include <kernel/host.h>
#include <kernel/umirq.h>

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

int host_open(const char *name, int flags, int mode) {
	return syscall(NR_OPEN, (int) name, flags, mode, 0, 0);
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

int host_sigaction(int signum, const struct host_sigaction *new,
		struct host_sigaction *old) {
	return syscall(NR_SIGACT, signum, (int) new, (int) old, 0, 0);
}

int host_dup2(int fd, int fd2) {
	return syscall(NR_DUP2, fd, fd2, 0, 0, 0);
}

void host_exit(int ret) {
	syscall(NR_EXIT, ret, 0, 0, 0, 0);
}

int host_close(int fd) {
	return syscall(NR_CLOSE, fd, 0, 0, 0, 0);
}

host_pid_t host_getpid(void) {
	return syscall(NR_GETPID, 0, 0, 0, 0, 0);
}

int host_pause(void) {
	return syscall(NR_PAUSE, 0, 0, 0, 0, 0);
}
