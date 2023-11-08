/**
 * @file
 * @brief EMBuild-side binding interface.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_H_
#define FRAMEWORK_MOD_EMBUILD_H_

#ifndef __EMBUILD__
# error "Do not include <mod/embuild.h> outside of EMBuild-generated code!"
#endif /* __EMBUILD__ */

#include <util/array.h>
#include <util/log.h>

#include <framework/mod/decls.h>
#include <framework/mod/types.h>
#include <framework/mod/self.h>
#include <framework/mod/options.h>

#include <framework/cmd/types.h>

#include <module/embox/framework/embuild.h>

/**
 * Defines a new package.
 *
 * @param package_nm
 *   Variable name to use.
 * @param package_name
 *   A string containing the package name.
 */
#define MOD_PACKAGE_DEF(package_nm, package_name) \
	__MOD_PACKAGE_DEF(package_nm, package_name)

/**
 * Defines a new mod. For the new mod the @link #mod corresponding @endlink
 * structure is allocated. Also two section-driven arrays are defined for the
 * mod#provides and mod#requires lists.
 *
 * @param runlevel_nr
 *   The runlevel number which module belongs to.
 * @param seq_num
 *   Sequential number of module on runlevel. Must be formated to pass
 *   literal sorting.
 * @param mod_package_nm
 *   The package variable name used in #MOD_PACKAGE_DEF().
 * @param pkg_name
 *   Literal string with package name.
 * @param mod_nm
 *   The last fragment of module name (without package).
 *
 * @note
 *   The variable name is computed as mod_package_nm##__##mod_nm and used to:
 *    - define dependencies with #MOD_DEP_DEF(),
 *    - refer the mod inside those compilation units that bind mod-specific
 *      @link #mod_info information @endlink using #MOD_INFO_BIND() or define
 *      members with #MOD_MEMBER_BIND().
 *   For the code generated by EMBuild this argument is the same as provided
 *   by the @c __EMBUILD_MOD__ macro for each mod at compilation time.
 */
#define MOD_DEF(runlevel_nr, seq_num, mod_package_nm, pkg_name, mod_nm) \
	_MOD_DEF(runlevel_nr, seq_num, mod_package_nm ## __ ## mod_nm, pkg_name, #mod_nm)

#define _MOD_DEF(runlevel_nr, seq_num, mod_nm, pkg_name, mod_name) \
	__MOD_DEF(mod_nm); \
	__MOD_BUILDINFO_DEF(mod_nm, pkg_name, mod_name); \
	__MOD_RUNLEVEL_BIND(runlevel_nr, seq_num, mod_nm);

#if OPTION_MODULE_GET(embox__framework__mod, BOOLEAN, security_label)
# define MOD_LABEL_DEF(mod_nm) \
	__MOD_LABEL_DEF(mod_nm)
# define MOD_SEC_LABEL_DEF(mod_nm) \
	__MOD_SEC_LABEL_DEF(mod_nm)
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
	__MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details)

/**
 * App-specific definitions.
 * @param mod_nm
 */
#define MOD_APP_DEF(mod_nm) \
	__MOD_APP_DEF(mod_nm)

/**
 * Defines a new dependency between two specified modules.
 *
 * @param mod_nm
 *   Symbol name of the module which depends on @a dep_nm.
 * @param dep_nm
 *   Symbol name of the module on which @a mod_nm depends.
 */
#define MOD_DEP_DEF(mod_nm, dep_nm) \
	__MOD_DEP_DEF(mod_nm, dep_nm)

/**
 * Defines a new 'content' relation between two specified modules.
 *
 * @param mod_nm
 *   Symbol name of the module which contents on @a content_nm.
 * @param dep_nm
 *   Symbol name of the module which is in @a mod_nm.
 */
#define MOD_CONTENTS_DEF(mod_nm, content_nm) \
	__MOD_CONTENTS_DEF(mod_nm, content_nm)

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
	__MOD_AFTER_DEP_DEF(mod_nm, dep_nm)

/* Default implementations */

#ifndef __MOD_PACKAGE_DEF
#define __MOD_PACKAGE_DEF(package_nm, package_name)
#endif /* __MOD_PACKAGE_DEF */

#ifndef __MOD_DEF
#define __MOD_DEF(mod_nm) \
	struct __mod_private __MOD_PRIVATE(mod_nm) __attribute__((weak)); \
	ARRAY_SPREAD_DEF_TERMINATED(const struct mod_member *, \
			__MOD_MEMBERS(mod_nm), NULL); \
	extern const struct mod_app __MOD_APP(mod_nm) \
			__attribute__ ((weak)); \
	extern const struct mod_build_info __MOD_BUILDINFO(mod_nm) \
			__attribute__((weak)); \
	\
	extern const struct mod __MOD(mod_nm); \
	const struct mod __MOD(mod_nm) __attribute__((weak)) = MOD_SELF_INIT(mod_nm, NULL); \
	\
	ARRAY_SPREAD_DECLARE(const struct mod * const,      \
			__mod_registry);                      \
	ARRAY_SPREAD_ADD(__mod_registry, &__MOD(mod_nm)) // TODO don't like it. -- Eldar
#endif /* __MOD_DEF */

#ifndef __MOD_BUILDINFO_DEF
#define __MOD_BUILDINFO_DEF(_mod_nm, _package_name, _mod_name) \
	extern const struct mod_label __MOD_LABEL(_mod_nm)       \
			__attribute__ ((weak));                          \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,   \
			__MOD_REQUIRES(_mod_nm), NULL);                  \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,   \
			__MOD_PROVIDES(_mod_nm), NULL);                  \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,   \
			__MOD_CONTENTS(_mod_nm), NULL);                  \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,   \
			__MOD_AFTER_DEPS(_mod_nm), NULL);                \
	const struct mod_build_info __MOD_BUILDINFO(_mod_nm) = { \
		.pkg_name   = _package_name,                         \
		.mod_name   = _mod_name,                             \
		.label      = &__MOD_LABEL(_mod_nm),                 \
		.requires   = __MOD_REQUIRES(_mod_nm),               \
		.provides   = __MOD_PROVIDES(_mod_nm),               \
		.after_deps = __MOD_AFTER_DEPS(_mod_nm),             \
		.contents   = __MOD_CONTENTS(_mod_nm),               \
	}
#endif /* __MOD_BUILDINFO_DEF */

#ifndef __MOD_RUNLEVEL_BIND
#define __MOD_RUNLEVEL_BIND(nr, seq_num, mod_nm) \
	ARRAY_SPREAD_DECLARE(const struct mod *,__mod_runlevel ## nr); \
	__ARRAY_SPREAD_ADD_ORDERED(__mod_runlevel ## nr, seq_num, &__MOD(mod_nm))
#endif /* __MOD_RUNLEVEL_BIND */

#ifndef __MOD_LABEL_DEF
#define __MOD_LABEL_DEF(mod_nm) \
	extern char __module_ ## mod_nm ## _text_vma;  \
	extern char __module_ ## mod_nm ## _text_len;  \
	extern const char __module_ ## mod_nm ## _text_md5sum[] __attribute__((weak));  \
	extern char __module_ ## mod_nm ## _rodata_vma; \
	extern char __module_ ## mod_nm ## _rodata_len; \
	extern const char __module_ ## mod_nm ## _rodata_md5sum[] __attribute__((weak));  \
	extern char __module_ ## mod_nm ## _data_vma; \
	extern char __module_ ## mod_nm ## _data_len; \
	extern char __module_ ## mod_nm ## _bss_vma;  \
	extern char __module_ ## mod_nm ## _bss_len;  \
	const struct mod_label __MOD_LABEL(mod_nm) = { \
		.text.vma   =          &__module_ ## mod_nm ## _text_vma,  \
		.text.len   = (size_t) &__module_ ## mod_nm ## _text_len,  \
		.text.md5sum = __module_ ## mod_nm ## _text_md5sum,  \
		.rodata.vma =          &__module_ ## mod_nm ## _rodata_vma, \
		.rodata.len = (size_t) &__module_ ## mod_nm ## _rodata_len, \
		.rodata.md5sum = __module_ ## mod_nm ## _rodata_md5sum,  \
		.data.vma   =          &__module_ ## mod_nm ## _data_vma, \
		.data.len   = (size_t) &__module_ ## mod_nm ## _data_len, \
		.bss.vma    =          &__module_ ## mod_nm ## _bss_vma, \
		.bss.len    = (size_t) &__module_ ## mod_nm ## _bss_len, \
	}
#endif /* __MOD_LABEL_DEF */

#ifndef __MOD_SEC_LABEL_DEF
#define __MOD_SEC_LABEL_DEF(mod_nm) \
	extern char __module_ ## mod_nm ## _text_vma; \
	extern char __module_ ## mod_nm ## _text_len; \
	extern char __module_ ## mod_nm ## _rodata_vma; \
	extern char __module_ ## mod_nm ## _rodata_len; \
	extern char __module_ ## mod_nm ## _data_vma; \
	extern char __module_ ## mod_nm ## _data_len; \
	extern char __module_ ## mod_nm ## _bss_vma;  \
	extern char __module_ ## mod_nm ## _bss_len;  \
	static const struct mod_sec_label __MOD_SEC_LABEL(mod_nm) = { \
		.label =  { \
			.text.vma   =          &__module_ ## mod_nm ## _text_vma,  \
			.text.len   = (size_t) &__module_ ## mod_nm ## _text_len,  \
			.rodata.vma =          &__module_ ## mod_nm ## _rodata_vma, \
			.rodata.len = (size_t) &__module_ ## mod_nm ## _rodata_len, \
			.data.vma   =          &__module_ ## mod_nm ## _data_vma, \
			.data.len   = (size_t) &__module_ ## mod_nm ## _data_len, \
			.bss.vma    =          &__module_ ## mod_nm ## _bss_vma, \
			.bss.len    = (size_t) &__module_ ## mod_nm ## _bss_len, \
		}, \
		.mod = &__MOD(mod_nm), \
	}; \
	ARRAY_SPREAD_DECLARE(const struct mod_sec_label *const, __mod_sec_labels); \
	ARRAY_SPREAD_ADD(__mod_sec_labels, &__MOD_SEC_LABEL(mod_nm))
#endif /* __MOD_SEC_LABEL_DEF */

#ifndef __MOD_CMD_DEF
#define __MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details) \
	const struct cmd_desc __MOD_CMD(mod_nm) = { \
		.name    = cmd_name, \
		.brief   = cmd_brief, \
		.details = cmd_details, \
	}
#endif /* __MOD_CMD_DEF */

#ifndef __MOD_APP_DEF
#define __MOD_APP_DEF(mod_nm) \
	extern char __module_ ## mod_nm ## _data_vma; \
	extern char __module_ ## mod_nm ## _data_lma; \
	extern char __module_ ## mod_nm ## _data_len; \
	extern char __module_ ## mod_nm ## _bss_vma;  \
	extern char __module_ ## mod_nm ## _bss_len;  \
	extern void *   __module_ ## mod_nm ## _build_deps_data_vma[]; \
	extern void *   __module_ ## mod_nm ## _build_deps_data_lma[]; \
	extern unsigned __module_ ## mod_nm ## _build_deps_data_len[]; \
	extern void *   __module_ ## mod_nm ## _build_deps_bss_vma[];  \
	extern unsigned __module_ ## mod_nm ## _build_deps_bss_len[];  \
	const struct mod_app __MOD_APP(mod_nm) = { \
		.data_vma =          &__module_ ## mod_nm ## _data_vma, \
		.data_lma =          &__module_ ## mod_nm ## _data_lma, \
		.data_sz  = (size_t) &__module_ ## mod_nm ## _data_len, \
		.bss      =          &__module_ ## mod_nm ## _bss_vma,  \
		.bss_sz   = (size_t) &__module_ ## mod_nm ## _bss_len,  \
		.build_dep_data_vma = __module_ ## mod_nm ## _build_deps_data_vma, \
		.build_dep_data_lma = __module_ ## mod_nm ## _build_deps_data_lma, \
		.build_dep_data_sz  = __module_ ## mod_nm ## _build_deps_data_len, \
		.build_dep_bss      = __module_ ## mod_nm ## _build_deps_bss_vma,  \
		.build_dep_bss_sz   = __module_ ## mod_nm ## _build_deps_bss_len,  \
	}
#endif /* __MOD_APP_DEF */

#ifndef __MOD_DEP_DEF
#define __MOD_DEP_DEF(mod_nm, dep_nm) \
	ARRAY_SPREAD_ADD(__MOD_REQUIRES(mod_nm), &__MOD(dep_nm)); \
	ARRAY_SPREAD_ADD(__MOD_PROVIDES(dep_nm), &__MOD(mod_nm))
#endif /* __MOD_DEP_DEF */

#ifndef __MOD_CONTENTS_DEF
#define __MOD_CONTENTS_DEF(mod_nm, content_nm) \
	ARRAY_SPREAD_ADD(__MOD_CONTENTS(mod_nm), &__MOD(content_nm))
#endif /* __MOD_CONTENTS_DEF */

#ifndef __MOD_AFTER_DEP_DEF
#define __MOD_AFTER_DEP_DEF(mod_nm, dep_nm) \
	ARRAY_SPREAD_ADD(__MOD_AFTER_DEPS(mod_nm), &__MOD(dep_nm));
#endif /* __MOD_AFTER_DEP_DEF */

#endif /* FRAMEWORK_MOD_EMBUILD_H_ */
