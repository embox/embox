/**
 * @file
 * @brief Mod-private namespace declarations.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 * @author Anton Kozlov
 *		- options support
 */

#ifndef FRAMEWORK_MOD_DECLS_H_
#define FRAMEWORK_MOD_DECLS_H_

#include <util/macro.h>

/*
 * Implementation note:
 * Many macros use some of their arguments to construct symbol names. This
 * imposes well-known restrictions to the input values of these arguments.
 * To prevent a confusion all such arguments are suffixed by '_nm' (name).
 */

/* Internal variable names inside mods-private namespace. */

#define __MOD(mod_nm)          MACRO_CONCAT(__mod__,          mod_nm)
#define __MOD_INFO(mod_nm)     MACRO_CONCAT(__mod_info__,     mod_nm)
#define __MOD_PRIVATE(mod_nm)  MACRO_CONCAT(__mod_private__,  mod_nm)
#define __MOD_PROVIDES(mod_nm) MACRO_CONCAT(__mod_provides__, mod_nm)
#define __MOD_REQUIRES(mod_nm) MACRO_CONCAT(__mod_requires__, mod_nm)
#define __MOD_MEMBERS(mod_nm)  MACRO_CONCAT(__mod_members__,  mod_nm)

#define __MOD_PACKAGE(pkg_nm)  MACRO_CONCAT(__mod_package__,  pkg_nm)

#define __OPTION_MODULE_GET(mod,type,name) \
		OPTION_##type##_##mod##__##name

#define OPTION_MODULE_GET(mod,type,name) \
	__OPTION_MODULE_GET(mod,type,name)

#define OPTION_GET(type,name) \
	OPTION_MODULE_GET(__EMBUILD_MOD__,type,name)

/* Performs additional stringification, handy in string options */
#define OPTION_STRING_GET(name) \
	MACRO_STRING(OPTION_GET(STRING,name))
#endif /* FRAMEWORK_MOD_DECLS_H_ */
