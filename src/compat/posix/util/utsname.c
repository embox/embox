/**
 * @file
 * @brief
 *
 * @date 13.09.11
 * @author Anton Bondarev
 */

#include <string.h>
#include <sys/utsname.h>

static struct utsname sys_name = {
	.sysname = "Embox",
	.nodename = "",
	.release = "0.3.3",
	.version = __DATE__ __TIME__,
	.machine = "Platform"
};

int uname(struct utsname *name) {
	memcpy(name, &sys_name, sizeof(sys_name));
	return 0;
}
