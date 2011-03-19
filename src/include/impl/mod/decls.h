/**
 * @file
 * @brief TODO
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef IMPL_MOD_DECLS_H_
#define IMPL_MOD_DECLS_H_

#include <util/macro.h>

/* Internal variable names inside mods-private namespace. */

#define __MOD(mod_nm)      MACRO_CONCAT(__mod__, mod_nm)
#define __MOD_INFO(mod_nm) MACRO_CONCAT(__mod_info__, mod_nm)

/* Internal declarations. */

#define __MOD_DECL(mod_nm) \
	extern const struct mod __MOD(mod_nm)

#define __MOD_INFO_DECL(mod_nm) \
	extern const struct __mod_info __MOD_INFO(mod_nm) __attribute__ ((weak))

#endif /* IMPL_MOD_DECLS_H_ */
