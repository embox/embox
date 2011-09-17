/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 */

#include <string.h>
#include <sys/utsname.h>

static struct utsname sys_name = {
		.sysname = "Embox",
		.nodename = "",
		.release = "",
		.version = "",
		.machine = "Platform"
};


int uname(struct utsname *name) {
	memcpy(name, &sys_name, sizeof(sys_name));
	return 0;
}
