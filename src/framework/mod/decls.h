/**
 * @file
 * @brief Mod-private namespace declarations.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_DECLS_H_
#define FRAMEWORK_MOD_DECLS_H_

#include <util/macro.h>

/* Internal variable names inside mods-private namespace. */

#define __MOD(mod_nm)      MACRO_CONCAT(__mod__, mod_nm)
#define __MOD_INFO(mod_nm) MACRO_CONCAT(__mod_info__, mod_nm)

#define __MOD_MEMBER_ARRAY(mod_nm, array_nm) \
	MACRO_CONCAT(__mod_##array_nm##__,mod_nm)

/* Internal declarations. */

#define __MOD_DECL(mod_nm) \
	extern const struct mod __MOD(mod_nm)

#define __MOD_INFO_DECL(mod_nm) \
	extern const struct __mod_info __MOD_INFO(mod_nm) __attribute__ ((weak))

#endif /* FRAMEWORK_MOD_DECLS_H_ */
