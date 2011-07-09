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

#define __MOD_DEF(mod_nm, package_nm, mod_name, mod_brief, mod_details) \
	extern const struct __mod_info __MOD_INFO(mod_nm)          \
			__attribute__ ((weak));                            \
	static const struct mod_package __MOD_PACKAGE(package_nm); \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,     \
			__MOD_REQUIRES(mod_nm), NULL);                     \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod *,     \
			__MOD_PROVIDES(mod_nm), NULL);                     \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct mod_member_info *, \
			__MOD_MEMBERS(mod_nm), NULL);                      \
	static struct __mod_private __MOD_PRIVATE(mod_nm);         \
	const struct mod __MOD(mod_nm) = {          \
		.private  = &__MOD_PRIVATE(mod_nm),     \
		.info     = &__MOD_INFO(mod_nm),        \
		.package  = &__MOD_PACKAGE(package_nm), \
		.name     = mod_name,                   \
		.brief    = mod_brief,                  \
		.details  = mod_details,                \
		.requires = __MOD_REQUIRES(mod_nm),     \
		.provides = __MOD_PROVIDES(mod_nm),     \
		.members  = __MOD_MEMBERS(mod_nm),      \
	};                                          \
	extern const struct mod *__mod_registry[];  \
	ARRAY_SPREAD_ADD(__mod_registry, &__MOD(mod_nm)) // TODO don't like it. -- Eldar

#define __MOD_DEP_DEF(mod_nm, dep_nm) \
	ARRAY_SPREAD_ADD(__MOD_REQUIRES(mod_nm), &__MOD(dep_nm)); \
	ARRAY_SPREAD_ADD(__MOD_PROVIDES(dep_nm), &__MOD(mod_nm))

#define __MOD_PACKAGE_DEF(package_nm, package_name) \
	static const struct mod_package __MOD_PACKAGE(package_nm) = { \
		.name = package_name,                                \
	}

#endif /* FRAMEWORK_MOD_EMBUILD_IMPL_H_ */
