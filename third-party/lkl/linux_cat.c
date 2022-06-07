/*
 *  Simple `cat` which uses i386 Linux syscall convention
 */

/*  Can't use `read`, `write` etc. because they are just wrappers from GLIBC which is not ported yet.
 *  `write` -> GLIBC implementation -> actual `sys_write` (INT 0x80)
 */
static inline long os_syscall(int syscall, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4) {
	long ret;
	__asm__ __volatile__(
	"int $0x80\n"
	: "=a"(ret)
	: "a"(syscall), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4)
	:
	);
	return ret;
}

// param_1 is app name, param_2 is a first argument
int main(char* param_1, char* param_2) {
	// sys_lseek
	os_syscall(19, 1, 0, 0, 0);

	char buf[20];
	long totalread = 0;

	// sys_open
	int fd = os_syscall(5, (unsigned long int)param_2, 0x0000, 0, 0);

	int nbytes = sizeof(buf);
	int bytes_read;
	// sys_read
	while((bytes_read = os_syscall(3, fd, (unsigned long int)buf, nbytes, 0)) > 0 ) {
		// sys_lseek
		os_syscall(19, 1, 0, 0, 0);
		// sys_write
		os_syscall(4, 1, (unsigned long int)buf, bytes_read, 0);
		// sys_fsync
		os_syscall(118, 1, 0, 0, 0);
	}

	// sys_exit
	os_syscall(1, 0, 0, 0, 0);
}
