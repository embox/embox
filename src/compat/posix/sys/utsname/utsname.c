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

#include <hal/arch.h>
#include <hal/cpu.h>

#include <framework/mod/options.h>

#define MODOPS_SYSTEM OPTION_STRING_GET(system)
#define MODOPS_HOSTNAME OPTION_STRING_GET(hostname)

static const struct utsname sys_name = {
	.sysname = MODOPS_SYSTEM,
	.nodename = MODOPS_HOSTNAME,
	.release = __EMBOX_VERSION__,
#ifdef SMP
	.version = " SMP "__DATE__ " " __TIME__,
#else /* !SMP */
	.version = __DATE__ " " __TIME__,
#endif /* SMP */
	.machine = __PLATFORM_ARCH
};


int uname(struct utsname *name) {
	memcpy(name, &sys_name, sizeof(sys_name));

	return 0;
}
