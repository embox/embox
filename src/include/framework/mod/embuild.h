/**
 * @file
 * @brief EMBuild-side binding interface.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_H_
#define FRAMEWORK_MOD_EMBUILD_H_

#include <stddef.h>

#include <framework/cmd/types.h>
#include <framework/mod/decls.h>
#include <framework/mod/ops.h>
#include <lib/libds/array.h>
#include <util/log.h>
#include <util/macro.h>

/**
 * Defines security labels.
 *
 * @param mod_nm
 *   Symbol name of the module
 */
/* clang-format off */
#define __MOD_LABEL_DEF(mod_nm)                                                      \
	extern char __module_ ## mod_nm ## _text_vma;                                    \
	extern char __module_ ## mod_nm ## _text_len;                                    \
	extern char __module_ ## mod_nm ## _rodata_vma;                                  \
	extern char __module_ ## mod_nm ## _rodata_len;                                  \
	extern char __module_ ## mod_nm ## _data_vma;                                    \
	extern char __module_ ## mod_nm ## _data_len;                                    \
	extern char __module_ ## mod_nm ## _bss_vma;                                     \
	extern char __module_ ## mod_nm ## _bss_len;                                     \
	extern const char __module_ ## mod_nm ## _text_md5sum[] __attribute__((weak));   \
	extern const char __module_ ## mod_nm ## _rodata_md5sum[] __attribute__((weak)); \
	const struct mod_label __MOD_LABEL(mod_nm) = {                                   \
	    .text.vma      =         &__module_##mod_nm##_text_vma,                      \
	    .text.len      = (size_t)&__module_##mod_nm##_text_len,                      \
	    .text.md5sum   =          __module_##mod_nm##_text_md5sum,                   \
	    .rodata.vma    =         &__module_##mod_nm##_rodata_vma,                    \
	    .rodata.len    = (size_t)&__module_##mod_nm##_rodata_len,                    \
	    .rodata.md5sum =          __module_##mod_nm##_rodata_md5sum,                 \
	    .data.vma      =         &__module_##mod_nm##_data_vma,                      \
	    .data.len      = (size_t)&__module_##mod_nm##_data_len,                      \
	    .bss.vma       =         &__module_##mod_nm##_bss_vma,                       \
	    .bss.len       = (size_t)&__module_##mod_nm##_bss_len,                       \
	}
/* clang-format on */

/**
 * Defines application specific labels.
 *
 * @param mod_nm
 *   Symbol name of the module
 */
/* clang-format off */
#define __MOD_APP_DEF(mod_nm)                                              \
	extern char __module_ ## mod_nm ## _data_vma;                          \
	extern char __module_ ## mod_nm ## _data_lma;                          \
	extern char __module_ ## mod_nm ## _data_len;                          \
	extern char __module_ ## mod_nm ## _bss_vma;                           \
	extern char __module_ ## mod_nm ## _bss_len;                           \
	extern void *   __module_ ## mod_nm ## _build_deps_data_vma[];         \
	extern void *   __module_ ## mod_nm ## _build_deps_data_lma[];         \
	extern unsigned __module_ ## mod_nm ## _build_deps_data_len[];         \
	extern void *   __module_ ## mod_nm ## _build_deps_bss_vma[];          \
	extern unsigned __module_ ## mod_nm ## _build_deps_bss_len[];          \
	const struct mod_app __MOD_APP(mod_nm) = {                             \
		.data_vma =          &__module_ ## mod_nm ## _data_vma,            \
		.data_lma =          &__module_ ## mod_nm ## _data_lma,            \
		.data_sz  = (size_t) &__module_ ## mod_nm ## _data_len,            \
		.bss      =          &__module_ ## mod_nm ## _bss_vma,             \
		.bss_sz   = (size_t) &__module_ ## mod_nm ## _bss_len,             \
		.build_dep_data_vma = __module_ ## mod_nm ## _build_deps_data_vma, \
		.build_dep_data_lma = __module_ ## mod_nm ## _build_deps_data_lma, \
		.build_dep_data_sz  = __module_ ## mod_nm ## _build_deps_data_len, \
		.build_dep_bss      = __module_ ## mod_nm ## _build_deps_bss_vma,  \
		.build_dep_bss_sz   = __module_ ## mod_nm ## _build_deps_bss_len,  \
	}
/* clang-format on */

/**
 * Defines a new dependency between two specified modules.
 *
 * @param mod_nm
 *   Symbol name of the module which depends on @a dep_nm.
 * @param dep_nm
 *   Symbol name of the module on which @a mod_nm depends.
 */
#define __MOD_DEP_DEF(mod_nm, dep_nm)                                 \
	ARRAY_SPREAD_DECLARE(const struct mod *, __MOD_DEPENDS(mod_nm));  \
	ARRAY_SPREAD_DECLARE(const struct mod *, __MOD_PROVIDES(dep_nm)); \
	extern const struct mod __MOD(mod_nm);                            \
	extern const struct mod __MOD(dep_nm);                            \
	ARRAY_SPREAD_ADD(__MOD_DEPENDS(mod_nm), &__MOD(dep_nm));          \
	ARRAY_SPREAD_ADD(__MOD_PROVIDES(dep_nm), &__MOD(mod_nm))

/**
 * Defines a logger.
 *
 * @param mod_nm
 *   Symbol name of the module
 * @param log_level
 *   Default logging level for module @a mod_nm
 */
#define __MOD_LOGGER_DEF(mod_nm, log_level)    \
	struct mod_logger __MOD_LOGGER(mod_nm) = { \
	    .mod_name = __MOD_NAME(mod_nm),        \
	    .level = log_level,                    \
	}

/**
 * Cmd-specific definitions.
 *
 * @param mod_nm
 *   Symbol name of the module.
 * @param cmd_name
 *   A string with command name.
 * @param cmd_brief
 *   A one-line string containing brief description.
 * @param cmd_details
 *   A string with detailed description.
 */
#define __MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details) \
	const struct cmd_desc __MOD_CMD(mod_nm) = {                 \
	    .name = cmd_name,                                       \
	    .brief = cmd_brief,                                     \
	    .details = cmd_details,                                 \
	}

/**
 * Init module structure.
 *
 * @param mod_nm
 *   Symbol name of the module.
 * @param mod_ops
 *   Module operations.
 * @param _pkg_name
 *   A string containing the package name.
 * @param _mod_name
 *   A string containing the module name.
 */
/* clang-format off */
#define __MOD_INIT(mod_nm, mod_ops, _pkg_name, _mod_name) { \
	    /* .ops      = */ mod_ops,                          \
	    /* .pkg_name = */ _pkg_name,                        \
	    /* .mod_name = */ _mod_name,                        \
	    /* .label    = */ &__MOD_LABEL(mod_nm),             \
	    /* .depends  = */ __MOD_DEPENDS(mod_nm),            \
	    /* .provides = */ __MOD_PROVIDES(mod_nm),           \
	    /* .data     = */ &__MOD_DATA(mod_nm),              \
	}
/* clang-format on */

#if USE_MOD_NAMES
#define _MOD_INIT(mod_nm, mod_ops) \
	__MOD_INIT(mod_nm, mod_ops, __MOD_PACKAGE(mod_nm), __MOD_NAME(mod_nm))
#else
#define _MOD_INIT(mod_nm, mod_ops) __MOD_INIT(mod_nm, mod_ops, NULL, NULL)
#endif

#define MOD_APP_DEF(mod_nm) __MOD_APP_DEF(mod_nm)

#if USE_CMD_DETAILS
#define MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details) \
	__MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details)
#else
#define MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details) \
	__MOD_CMD_DEF(mod_nm, cmd_name, NULL, NULL)
#endif

#if USE_MOD_LABELS
#define MOD_LABEL_DEF(mod_nm) __MOD_LABEL_DEF(mod_nm)
#else
#define MOD_LABEL_DEF(mod_nm)
#endif

#if USE_MOD_LOGGER
#define MOD_LOGGER_DEF(mod_nm, log_level) __MOD_LOGGER_DEF(mod_nm, log_level)
#else
#define MOD_LOGGER_DEF(mod_nm, log_level)
#endif

#if USE_MOD_DEPENDS
#define MOD_DEP_DEF(mod_nm, dep_nm) __MOD_DEP_DEF(mod_nm, dep_nm)
#else
#define MOD_DEP_DEF(mod_nm, dep_nm)
#endif

#if USE_MOD_DEPENDS
#define MOD_DEF(mod_nm)                                                      \
	ARRAY_SPREAD_DEF_TERMINATED(const struct mod *, __MOD_DEPENDS(mod_nm),   \
	    NULL);                                                               \
	ARRAY_SPREAD_DEF_TERMINATED(const struct mod *, __MOD_PROVIDES(mod_nm),  \
	    NULL);                                                               \
	extern struct mod_data __MOD_DATA(mod_nm) __attribute__((weak));         \
	extern const struct mod_label __MOD_LABEL(mod_nm) __attribute__((weak)); \
	const struct mod __MOD(mod_nm)                                           \
	    __attribute__((weak)) = _MOD_INIT(mod_nm, NULL)
#else
#define MOD_DEF(mod_nm)
#endif

#endif /* FRAMEWORK_MOD_EMBUILD_H_ */
