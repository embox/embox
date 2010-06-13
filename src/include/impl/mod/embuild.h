/**
 * @file
 * @brief TODO
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef __EMBUILD__
# error "Do not include <embuild/mod.h> outside of EMBuild-generated code!"
#endif /* __EMBUILD__ */

#include "decls.h"
#include "array.h"
#include "types.h"

#define __MOD_NAME_DEF(s_mod, mod_name) \
	const char __MOD_NAME(s_mod)[] = mod_name

#define __MOD_PRIVATE_DEF(s_mod) \
	static struct mod_private __MOD_PRIVATE(s_mod)

#define __MOD_DEF(s_mod, s_mod_pkg, mod_name) \
		__MOD_INFO_DECL(s_mod); \
		__MOD_PACKAGE_DECL(s_mod_pkg); \
		__MOD_ARRAY_DEF(s_mod, requires); \
		__MOD_ARRAY_DEF(s_mod, provides); \
		__MOD_NAME_DEF(s_mod, mod_name); \
		__MOD_PRIVATE_DEF(s_mod); \
		const struct mod __MOD(s_mod) = { \
				.private  = &__MOD_PRIVATE(s_mod), \
				.info      = (struct mod_info *) &__MOD_INFO(s_mod), \
				.package  = (struct mod_package *) &__MOD_PACKAGE(s_mod_pkg), \
				.name     = __MOD_NAME(s_mod), \
				.requires = (struct mod **) &__MOD_ARRAY(s_mod, requires), \
				.provides = (struct mod **) &__MOD_ARRAY(s_mod, provides), \
			}

#define __MOD_DEP_DEF(s_mod, s_dep) \
		__MOD_DECL(s_mod); \
		__MOD_DECL(s_dep); \
		__MOD_ARRAY_ADD(s_mod, requires, s_dep); \
		__MOD_ARRAY_ADD(s_dep, provides, s_mod)

#define __MOD_PACKAGE_DEF(s_package, package_name) \
		const struct mod_package __MOD_PACKAGE(s_package) = { \
				.name = package_name, \
			}

// TODO this should be removed at all. -- Eldar
#define MOD_ROOT_DEF(s_mod) \
	static const struct mod *__mod_root__##s_mod \
		__attribute__ ((used, section(".mod.rodata"))) = &__MOD(s_mod)

