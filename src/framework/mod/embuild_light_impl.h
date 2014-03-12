/**
 * @file
 * @brief EMBuild-side binding interface.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_
#define FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_

#if 0
#include <util/array.h>

#include "decls.h"
#include "types.h"
#include "options.h"
#endif

#define __MOD_DEF(mod_nm, package_nm, mod_name)

/**
 * Cmd-specific definitions.
 *
 * @param mod_nm
 * @param cmd_name
 *   A string with command name.
 * @param cmd_brief
 *   A one-line string containing brief description.
 * @param cmd_details
 *   A string with detailed description.
 */
#define MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details) \
	const struct mod_cmd __MOD_CMD(mod_nm) = { \
		.name    = cmd_name,   \
		.brief   = cmd_brief,   \
		.details = cmd_details, \
	}

/**
 * Defines a new package.
 *
 * @param package_nm
 *   Variable name to use.
 * @param package_name
 *   A string containing the package name.
 */
#define MOD_PACKAGE_DEF(package_nm, package_name)

#endif /* FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_ */
