/**
 * @file
 * @brief EMBuild-side binding interface.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_FULL_IMPL_H_
#define FRAMEWORK_MOD_EMBUILD_FULL_IMPL_H_

#include <util/array.h>

#include <framework/mod/decls.h>
#include <framework/mod/types.h>
#include <framework/mod/options.h>

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
		.package    = &__MOD_PACKAGE(package_nm), \
		.cmd        = &__MOD_CMD(mod_nm),         \
		.app        = &__MOD_APP(mod_nm),         \
		.label      = &__MOD_LABEL(mod_nm),       \
		.name       = mod_name,                   \
		.requires   = __MOD_REQUIRES(mod_nm),     \
		.provides   = __MOD_PROVIDES(mod_nm),     \
		.after_deps = __MOD_AFTER_DEPS(mod_nm),   \
		.contents   = __MOD_CONTENTS(mod_nm),     \
		.members    = __MOD_MEMBERS(mod_nm),      \
	};                                            \
	ARRAY_SPREAD_DECLARE(const struct mod *,      \
			__mod_registry);                      \
	ARRAY_SPREAD_ADD(__mod_registry, &__MOD(mod_nm)) // TODO don't like it. -- Eldar

#if OPTION_MODULE_DEFINED(embox__framework__mod_full, BOOLEAN, security_label) \
	&& OPTION_MODULE_GET(embox__framework__mod_full, BOOLEAN, security_label)

#define MOD_LABEL_DEF(mod_nm) \
	/* extern char __module_ ## mod_nm ## _text_vma;  */\
	/* extern char __module_ ## mod_nm ## _text_len;  */\
	extern char __module_ ## mod_nm ## _rodata_vma; \
	extern char __module_ ## mod_nm ## _rodata_len; \
	extern char __module_ ## mod_nm ## _data_vma; \
	extern char __module_ ## mod_nm ## _data_len; \
	extern char __module_ ## mod_nm ## _bss_vma;  \
	extern char __module_ ## mod_nm ## _bss_len;  \
	const struct mod_label __MOD_LABEL(mod_nm) = { \
		/* .text.vma   =          &__module_ ## mod_nm ## _text_vma,  */\
		/* .text.len   = (size_t) &__module_ ## mod_nm ## _text_len,  */\
		.rodata.vma =          &__module_ ## mod_nm ## _rodata_vma, \
		.rodata.len = (size_t) &__module_ ## mod_nm ## _rodata_len, \
		.data.vma   =          &__module_ ## mod_nm ## _data_vma, \
		.data.len   = (size_t) &__module_ ## mod_nm ## _data_len, \
		.bss.vma    =          &__module_ ## mod_nm ## _bss_vma, \
		.bss.len    = (size_t) &__module_ ## mod_nm ## _bss_len, \
	}

#define MOD_SEC_LABEL_DEF(mod_nm) \
	extern char __module_ ## mod_nm ## _rodata_vma; \
	extern char __module_ ## mod_nm ## _rodata_len; \
	extern char __module_ ## mod_nm ## _data_vma; \
	extern char __module_ ## mod_nm ## _data_len; \
	extern char __module_ ## mod_nm ## _bss_vma;  \
	extern char __module_ ## mod_nm ## _bss_len;  \
	static const struct mod_sec_label __MOD_SEC_LABEL(mod_nm) = { \
		.label =  { \
			/* .text.vma   =          &__module_ ## mod_nm ## _text_vma,  */\
			/* .text.len   = (size_t) &__module_ ## mod_nm ## _text_len,  */\
			.rodata.vma =          &__module_ ## mod_nm ## _rodata_vma, \
			.rodata.len = (size_t) &__module_ ## mod_nm ## _rodata_len, \
			.data.vma   =          &__module_ ## mod_nm ## _data_vma, \
			.data.len   = (size_t) &__module_ ## mod_nm ## _data_len, \
			.bss.vma    =          &__module_ ## mod_nm ## _bss_vma, \
			.bss.len    = (size_t) &__module_ ## mod_nm ## _bss_len, \
		}, \
		.mod = &__MOD(mod_nm), \
	}; \
	ARRAY_SPREAD_DECLARE(const struct mod_sec_label *, __mod_sec_labels); \
	ARRAY_SPREAD_ADD(__mod_sec_labels, &__MOD_SEC_LABEL(mod_nm))


#else
# define MOD_LABEL_DEF(mod_nm)
# define MOD_SEC_LABEL_DEF(mod_nm)
#endif

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

/**
 * Defines a new dependency between two specified modules.
 *
 * @param mod_nm
 *   Symbol name of the module which depends on @a dep_nm.
 * @param dep_nm
 *   Symbol name of the module on which @a mod_nm depends.
 */
#define MOD_DEP_DEF(mod_nm, dep_nm) \
	ARRAY_SPREAD_ADD(__MOD_REQUIRES(mod_nm), &__MOD(dep_nm)); \
	ARRAY_SPREAD_ADD(__MOD_PROVIDES(dep_nm), &__MOD(mod_nm))


#define MOD_CONTENTS_DEF(mod_nm, content_nm) \
	ARRAY_SPREAD_ADD(__MOD_CONTENTS(mod_nm), &__MOD(content_nm))


/**
 * Defines new after-loading dependency of modules
 * @param mod_nm
 *   Symbol name of the module after which @a dep_nm
 *   should be loaded
 * @param dep_nm
 *   Symbol name of the module that should be loaded
 *   after @a mod_nm
 */
#define MOD_AFTER_DEP_DEF(mod_nm, dep_nm) \
	ARRAY_SPREAD_ADD(__MOD_AFTER_DEPS(mod_nm), &__MOD(dep_nm)); \


/**
 * Defines a new package.
 *
 * @param package_nm
 *   Variable name to use.
 * @param package_name
 *   A string containing the package name.
 */
#define MOD_PACKAGE_DEF(package_nm, package_name) \
	static const struct mod_package __MOD_PACKAGE(package_nm) = { \
		.name = package_name,                                \
	}


#endif /* FRAMEWORK_MOD_EMBUILD_FULL_IMPL_H_ */
