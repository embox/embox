/**
 * @file
 * @brief EMBuild-side binding interface.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_
#define FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_

#include <util/array.h>

#include <framework/mod/decls.h>
#include <framework/mod/types.h>
#include <framework/mod/options.h>

#define MOD_PACKAGE_DEF(package_nm, package_name) \
	static const struct mod_package __MOD_PACKAGE(package_nm) = { \
		.name = package_name,                                \
	}

#define __MOD_DEF(mod_nm, package_nm, mod_name) \
	extern const struct mod_info __MOD_INFO(mod_nm)               \
			__attribute__ ((weak));                               \
	extern const struct mod_cmd __MOD_CMD(mod_nm)                 \
			__attribute__ ((weak));                               \
	extern const struct mod_app __MOD_APP(mod_nm)                 \
			__attribute__ ((weak));                               \
	extern const struct mod_label __MOD_LABEL(mod_nm)             \
			__attribute__ ((weak));                               \
	static const struct mod_package __MOD_PACKAGE(package_nm);    \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,        \
			__MOD_REQUIRES(mod_nm), NULL);                        \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,        \
			__MOD_PROVIDES(mod_nm), NULL);                        \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,        \
			__MOD_AFTER_DEPS(mod_nm), NULL);                      \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,        \
			__MOD_CONTENTS(mod_nm), NULL);                        \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod_member *, \
			__MOD_MEMBERS(mod_nm), NULL);                         \
	static struct __mod_private __MOD_PRIVATE(mod_nm);            \
	const struct mod __MOD(mod_nm) = {            \
		.priv       = &__MOD_PRIVATE(mod_nm),     \
		.info       = &__MOD_INFO(mod_nm),        \
		.cmd        = &__MOD_CMD(mod_nm),         \
		.app        = &__MOD_APP(mod_nm),         \
		.name       = mod_name,                   \
		.members    = __MOD_MEMBERS(mod_nm),      \
		.package    = &__MOD_PACKAGE(package_nm), \
	};                                            \
	ARRAY_SPREAD_DECLARE(const struct mod *,      \
			__mod_registry);                      \
	ARRAY_SPREAD_ADD(__mod_registry, &__MOD(mod_nm)) // TODO don't like it. -- Eldar

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
#define MOD_LABEL_DEF(mod_nm)
#define MOD_SEC_LABEL_DEF(mod_nm)

#endif /* FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_ */
