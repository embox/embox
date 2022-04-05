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

	// Write to STDOUT
	os_syscall(4, 1, (unsigned long int)string, 13, 0);

	// Exit
	os_syscall(1, 0, 0, 0, 0);
}
