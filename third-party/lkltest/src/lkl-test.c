// $ gcc lkl-test.c -llkl
// $ ./a.out

#include <stdio.h>
#include <lkl_host.h>
#include <lkl.h>

int main() {
	#define access_rights 0721

	long ret;
	char cmdline[16];

	snprintf(cmdline, sizeof(cmdline), "mem=64M loglevel=8"); // loglevel=8 is from tests
	ret = lkl_start_kernel(&lkl_host_ops, cmdline);

	if (ret) {
		fprintf(stderr, "can't start kernel: %s\n", lkl_strerror(ret));
		return 1;
	}

	if (!ret) {
		fprintf(stderr, "kernel started: %s\n", lkl_strerror(ret));
	}

	printf("lkl_start_kernel returned %ld\n", ret);

	// file: fd=0, file2: fd=1
	lkl_sys_creat("/file", access_rights);
	lkl_sys_creat("/file2", access_rights);

	static const char wrbuf[] = "test";
	ret = lkl_sys_write(0, wrbuf, sizeof(wrbuf));

	// should be size of written data
	printf("lkl_sys_write returned %ld\n", ret);

	if (ret < 0) {
		lkl_printf("can't lkl_sys_write: %s\n", lkl_strerror(ret));
	}

	return 0;
}
