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

/*
 * Implementation note:
 * Many macros uses some of their arguments to construct symbol names. This
 * imposes well-known restrictions to the input values of these arguments.
 * To prevent a confusion all such arguments are suffixed by '_nm' (name).
 */

#define __MOD_PRIVATE(mod_nm)         __mod_private__##mod_nm
#define __MOD_PACKAGE(package_nm)     __mod_package__##package_nm
#define __MOD_ARRAY(mod_nm, array_nm) __mod_##array_nm##__##mod_nm

#define __MOD_PRIVATE_DEF(mod_nm) \
	static struct __mod_private __MOD_PRIVATE(mod_nm)

#define __MOD_PACKAGE_DECL(package_nm) \
	static const struct mod_package __MOD_PACKAGE(package_nm)

#define __MOD_ARRAY_MEMBER_DEF(mod_nm, array_nm) \
	ARRAY_SPREAD_DEF_TERMINATED( \
			static const struct mod_members_info *, \
			__MOD_MEMBER_ARRAY(mod_nm, array_nm), NULL)

#define __MOD_ARRAY_DEF(mod_nm, array_nm) \
	ARRAY_SPREAD_DEF_TERMINATED( \
			static const struct mod *, __MOD_ARRAY(mod_nm, array_nm), NULL)

#define __MOD_ARRAY_ADD(mod_nm, array_nm, mod_entry_nm) \
	extern const struct mod *__MOD_ARRAY(mod_nm, array_nm)[]; \
	ARRAY_SPREAD_ADD(__MOD_ARRAY(mod_nm, array_nm), &__MOD(mod_entry_nm))

/* Macro API impl. */

#define __MOD_DEF(mod_nm, mod_package_nm, mod_name, mod_brief, mod_details) \
	__MOD_INFO_DECL(mod_nm);                            \
	__MOD_PACKAGE_DECL(mod_package_nm);                 \
	__MOD_ARRAY_DEF(mod_nm, requires);                  \
	__MOD_ARRAY_DEF(mod_nm, provides);                  \
	__MOD_ARRAY_MEMBER_DEF(mod_nm, members);            \
	__MOD_PRIVATE_DEF(mod_nm);                          \
	extern const struct mod *__mod_registry[];          \
	const struct mod __MOD(mod_nm) = {                  \
		.private  = &__MOD_PRIVATE(mod_nm),             \
		.info     = &__MOD_INFO(mod_nm),                \
		.package  = &__MOD_PACKAGE(mod_package_nm),     \
		.name     = mod_name,                           \
		.brief    = mod_brief,                          \
		.details  = mod_details,                        \
		.requires = __MOD_ARRAY(mod_nm, requires),      \
		.provides = __MOD_ARRAY(mod_nm, provides),      \
		.members  = __MOD_MEMBER_ARRAY(mod_nm, members),\
	};                                                  \
	ARRAY_SPREAD_ADD_NAMED(__mod_registry,              \
			__MOD(mod_nm##ptr), &__MOD(mod_nm)) // TODO don't like it. -- Eldar

#define __MOD_DEP_DEF(mod_nm, dep_nm) \
		__MOD_DECL(mod_nm);                        \
		__MOD_DECL(dep_nm);                        \
		__MOD_ARRAY_ADD(mod_nm, requires, dep_nm); \
		__MOD_ARRAY_ADD(dep_nm, provides, mod_nm)

#define __MOD_PACKAGE_DEF(package_nm, package_name) \
	static const struct mod_package __MOD_PACKAGE(package_nm) = { \
		.name = package_name,                                \
	}

#endif /* FRAMEWORK_MOD_EMBUILD_IMPL_H_ */
