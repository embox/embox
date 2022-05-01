/*
 *  Simple `ls` which uses i386 Linux syscall convention
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

/*  If we used GLIBC we would call `readdir()` wrapper instead of direct `getdents64` syscall.
 *  GLIBC provides `dirent` strcut for `readdir()`. We will define `linux_dirent64` for `getdents64` here.
 */
typedef long long    __s64;
typedef unsigned long long    __u64;

typedef __u64    u64;
typedef __s64    s64;

struct linux_dirent64 {
	u64		d_ino;
	s64		d_off;
	unsigned short	d_reclen;
	unsigned char	d_type;
	char		d_name[0];
};

// param_1 is app name, param_2 is a first argument
int main(char* param_1, char* param_2) {
	// sys_lseek
	os_syscall(19, 1, 0, 0, 0);
	// sys_write
	os_syscall(4, 1, (unsigned long int)"TYPE          NAME\n", 19, 0);
	//sys_fsync
	os_syscall(118, 1, 0, 0, 0);

	// sys_open
	int fd = os_syscall(5, (unsigned long int)param_2, 0x0000, 0, 0);

	char buf[1024];
	struct linux_dirent64 *de;
	de = (struct linux_dirent64*) buf;

	// sys_getdents64
	long buf_len = os_syscall(220, fd, (unsigned long int)de, sizeof(buf), 0);

	// Print TYPE
	char *pos;
	for (pos = buf; pos - buf < buf_len; pos += de->d_reclen) {
		de = (struct linux_dirent64*)pos;
		// sys_lseek
		os_syscall(19, 1, 0, 0, 0);

		switch (de->d_type) {
			case 1:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"fifo          ", 14, 0);
				break;
			case 2:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"char dev      ", 14, 0);
				break;
			case 4:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"dir           ", 14, 0);
				break;
			case 6:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"blk dev       ", 14, 0);
				break;
			case 8:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"file          ", 14, 0);
				break;
			case 10:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"symlink       ", 14, 0);
				break;
			case 12:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"socket        ", 14, 0);
				break;
			default:
				// sys_write
				os_syscall(4, 1, (unsigned long int)"unknown       ", 14, 0);
				break;
		}
		// sys_fsync
		os_syscall(118, 1, 0, 0, 0);

		// Print NAME
		int len = 0;
		while(de->d_name[len+1]) {
			len++;
		}

		// sys_lseek
		os_syscall(19, 1, 0, 0, 0);
		// sys_write
		os_syscall(4, 1, (unsigned long int)de->d_name, len+1, 0);
		os_syscall(4, 1, (unsigned long int)"\n", 1, 0);
		// sys_fsync
		os_syscall(118, 1, 0, 0, 0);
	}

	// sys_exit
	os_syscall(1, 0, 0, 0, 0);
}
