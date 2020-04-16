#include <sys/syscall.h>

int a = 9;
int b = 9;

int x() {
	return 7;
}

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
    // write(1, "hi\n", 3);
	// syscall(SYS_write, 1, "hi\n", 3);
	os_syscall(SYS_write, 1, "hi\n", 3, 0);
    int p = x();
	return a + p + x();
}
