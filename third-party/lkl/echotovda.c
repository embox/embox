#include <stdio.h>
#include <lkl_host.h>
#include <lkl.h>

static inline long os_syscall(int syscall,
							  unsigned long arg1, unsigned long arg2,
							  unsigned long arg3, unsigned long arg4) {
	long ret;
	__asm__ __volatile__(
	"int $0x80\n"
	: "=a"(ret)
	: "a"(syscall), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4)
	:
	);
	return ret;
}

int main(int argc, char **argv) {
	/*
	Try to get FD for special file /vda.
	This file should have been created in src/kernel/task/lkl_task.c.
	*/

	long fd = os_syscall(5, "/vda", LKL_O_RDWR, 0, 0);

	if (fd < 0) {
		printf("Can't open /vda\n");
		return 1;
	}

	// Count input length
	int len = 0;
	while (argv[1][len] != '\0') {
		len++;
	}

	// Write to /vda
	long ret_write = os_syscall(4, fd, &argv[1][0], len, 0);

	if (ret_write < 0) {
		printf("Can't write to /vda");
		return 1;
	}

	/*
	fsync() LKL's /vda device.
	/vda driver in src/kernel/task/lkl_task.c will be triggered immediately.
	*/
	long ret_fsync = os_syscall(118, fd, 0, 0, 0);

	if (ret_fsync < 0) {
		printf("Can't fsync LKL's /vda");
		return 1;
	}
}
