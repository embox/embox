/**
 *  Simple Hello World which uses i386 Linux syscall convention
 */

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
	char* string = "Hello World!\n";

	/* We use special file '/vda' (it points to virtual block device) for STDOUT in LKL.
	 * '/vda' handler redirects everything to Embox terminal.
	 */

	// 'lseek(1, 0, 0)' is required for '/vda'
	//   because it is shared between LKL's kthreads (LKL has one process).
	// This doesn't matter for Linux 'tty' device.
	os_syscall(19, 1, 0, 0, 0);

	// Write to STDOUT.
	os_syscall(4, 1, (unsigned long int)string, 13, 0);

	// 'fsync(1)' is required to trigger '/vda' handler in LKL.
	// This doesn't matter for Linux 'tty' device.
	os_syscall(118, 1, 0, 0, 0);

	// Exit.
	os_syscall(1, 0, 0, 0, 0);
}
