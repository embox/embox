#include <stdio.h>
#include <lkl_host.h>
#include <lkl.h>
#include <kernel/task.h>

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
	/* Temporary.
	 * In the future 'lkl_task = 1' and 'lkl_tls_key = NULL'
	 * will be assigned in 'load_app'.
	 */
	task_self()->lkl_task = 1;
	task_self()->lkl_tls_key = NULL;

	printf("Started 'echotovda' execution.\n");
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());

	/*
	 * Try to get FD for special file /vda.
	 * This file should have been created in src/kernel/task/lkl_task.c.
	*/
	long fd = os_syscall(5, "/vda", LKL_O_RDWR, 0, 0);

	if (fd < 0) {
		printf("Can't open /vda\n");
		return 1;
	}
	printf("lkl_sys_open() for /vda returned %d.\n", fd);
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());

	// SIGCHLD was here
	// int clone_ret = lkl_sys_clone(0x00000100|0x00000200|0x00000400|0x00000800|0x00010000|0x00040000|0x00080000|0x00100000|0x00200000, 0 , NULL, NULL, 0);
	// printf("lkl_sys_clone() returned %d.\n", clone_ret);

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

	printf("lkl_sys_write() for /vda returned %d.\n", ret_write);
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());

	/* fsync() LKL's /vda device.
	 * /vda driver in src/kernel/task/lkl_task.c will be triggered immediately.
	 */
	long ret_fsync = os_syscall(118, fd, 0, 0, 0);

	if (ret_fsync < 0) {
		printf("Can't fsync LKL's /vda");
		return 1;
	}

	printf("lkl_sys_fsync() for /vda returned %d.\n", ret_fsync);
	printf("lkl_sys_gettid() = %d.\n", lkl_sys_gettid());
}
