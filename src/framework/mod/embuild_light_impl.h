/**
 * @file
 * @brief EMBuild-side binding interface.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_
#define FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_

#include <framework/mod/decls.h>
#include <framework/mod/types.h>
#include <framework/mod/options.h>

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
 * App-specific definitions.
 * @param mod_nm
 */
#define MOD_APP_DEF(mod_nm) \
	extern char __module_ ## mod_nm ## _data_vma; \
	extern char __module_ ## mod_nm ## _data_len; \
	extern char __module_ ## mod_nm ## _bss_vma;  \
	extern char __module_ ## mod_nm ## _bss_len;  \
	const struct mod_app __MOD_APP(mod_nm) = { \
		.data    =          &__module_ ## mod_nm ## _data_vma, \
		.data_sz = (size_t) &__module_ ## mod_nm ## _data_len, \
		.bss     =          &__module_ ## mod_nm ## _bss_vma,  \
		.bss_sz  = (size_t) &__module_ ## mod_nm ## _bss_len,  \
	}

#define MOD_DEP_DEF(mod_nm, dep_nm)
#define MOD_CONTENTS_DEF(mod_nm, content_nm)
#define MOD_AFTER_DEP_DEF(mod_nm, dep_nm)
#define MOD_PACKAGE_DEF(package_nm, package_name)
#define MOD_LABEL_DEF(mod_nm)
#define MOD_SEC_LABEL_DEF(mod_nm)

#endif /* FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_ */
