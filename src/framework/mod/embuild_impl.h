/**
 * @file
 * @brief Implementation of macros used by EMBuild DI model code generator.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_IMPL_H_
#define FRAMEWORK_MOD_EMBUILD_IMPL_H_

#ifndef __EMBUILD__
# error "Do not include <mod/embuild.h> outside of EMBuild-generated code!"
#endif /* __EMBUILD__ */

#include <util/array.h>

#include "decls.h"
#include "types.h"

/* Macro API impl. */

#define __MOD_DEF(mod_nm, package_nm, mod_name) \
	extern const struct mod_info __MOD_INFO(mod_nm)               \
			__attribute__ ((weak));                               \
	extern const struct mod_cmd __MOD_CMD(mod_nm)                 \
			__attribute__ ((weak));                               \
	extern const struct mod_app __MOD_APP(mod_nm)                 \
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
		.name       = mod_name,                   \
		.requires   = __MOD_REQUIRES(mod_nm),     \
		.provides   = __MOD_PROVIDES(mod_nm),     \
		.after_deps = __MOD_AFTER_DEPS(mod_nm),   \
		.contents   = __MOD_CONTENTS(mod_nm),     \
		.members    = __MOD_MEMBERS(mod_nm),      \
	};                                            \
	extern const struct mod *__mod_registry[];    \
	ARRAY_SPREAD_ADD(__mod_registry, &__MOD(mod_nm)) // TODO don't like it. -- Eldar

#define __MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details) \
	const struct mod_cmd __MOD_CMD(mod_nm) = { \
		.name    = cmd_name,   \
		.brief   = cmd_brief,   \
		.details = cmd_details, \
	}

#define __MOD_APP_DEF(mod_nm) \
	extern char __app_ ## mod_nm ## _data_vma; \
	extern char __app_ ## mod_nm ## _data_len; \
	extern char __app_ ## mod_nm ## _bss_vma;  \
	extern char __app_ ## mod_nm ## _bss_len;  \
	const struct mod_app __MOD_APP(mod_nm) = { \
		.data    =          &__app_ ## mod_nm ## _data_vma, \
		.data_sz = (size_t) &__app_ ## mod_nm ## _data_len, \
		.bss     =          &__app_ ## mod_nm ## _bss_vma,  \
		.bss_sz  = (size_t) &__app_ ## mod_nm ## _bss_len,  \
	}

#define __MOD_DEP_DEF(mod_nm, dep_nm) \
	ARRAY_SPREAD_ADD(__MOD_REQUIRES(mod_nm), &__MOD(dep_nm)); \
	ARRAY_SPREAD_ADD(__MOD_PROVIDES(dep_nm), &__MOD(mod_nm))

#define __MOD_CONTENTS_DEF(mod_nm, content_nm) \
	ARRAY_SPREAD_ADD(__MOD_CONTENTS(mod_nm), &__MOD(content_nm))

#define __MOD_AFTER_DEP_DEF(mod_nm, dep_nm) \
	ARRAY_SPREAD_ADD(__MOD_AFTER_DEPS(mod_nm), &__MOD(dep_nm)); \

#define __MOD_PACKAGE_DEF(package_nm, package_name) \
	static const struct mod_package __MOD_PACKAGE(package_nm) = { \
		.name = package_name,                                \
	}

#endif /* FRAMEWORK_MOD_EMBUILD_IMPL_H_ */
