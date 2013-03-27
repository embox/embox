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
#include <hal/arch.h>
#include <kernel/cpu.h>

#define MODOPS_RELEASE OPTION_STRING_GET(release)
#define MODOPS_SYSTEM OPTION_STRING_GET(system)
#define MODOPS_HOSTNAME OPTION_STRING_GET(hostname)

static struct utsname sys_name = {
	.sysname = MODOPS_SYSTEM,
	.nodename = MODOPS_HOSTNAME,
	.release = MODOPS_RELEASE,
#ifdef SMP
	.version = " SMP "__DATE__ " " __TIME__,
#else
	.version = __DATE__ " " __TIME__,
#endif
	.machine = __PLATFORM_ARCH
};


int uname(struct utsname *name) {
	memcpy(name, &sys_name, sizeof(sys_name));

	return 0;
}
