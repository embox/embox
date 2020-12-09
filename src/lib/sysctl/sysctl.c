/**
 * @file
 * @brief
 *
 * @date 2.12.2020
 * @author Suraaj K S
 */

#include <string.h>

#include <lib/sysctl.h>

#include <framework/mod/options.h>

#define MODOPS_SYSTEM OPTION_STRING_GET(system)
#define MODOPS_PLATFORM OPTION_STRING_GET(platform)
#define MODOPS_PROCESSOR OPTION_STRING_GET(processor)

static const struct sysct sys_name = {
	.processor = MODOPS_PROCESSOR,
	.platform = MODOPS_PLATFORM,
	.system = MODOPS_SYSTEM,
};


int sysctl(struct sysct *name) {
	memcpy(name, &sys_name, sizeof(sys_name));

	return 0;
}
