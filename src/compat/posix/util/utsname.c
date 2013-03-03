/**
 * @file
 * @brief
 *
 * @date 13.09.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <string.h>
#include <sys/utsname.h>
#include <framework/mod/options.h>

#define MODOPS_RELEASE OPTION_STRING_GET(release)
#define MODOPS_PLATFORM OPTION_STRING_GET(platform)

static struct utsname sys_name = {
	.sysname = "Embox",
	.nodename = "embox",
	.release = MODOPS_RELEASE,
	.version = __DATE__ " " __TIME__,
	.machine = MODOPS_PLATFORM
};

int uname(struct utsname *name) {
	memcpy(name, &sys_name, sizeof(sys_name));
	return 0;
}
