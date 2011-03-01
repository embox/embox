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

#define __MOD(s_mod)      MACRO_CONCAT(__mod__,s_mod)
#define __MOD_INFO(s_mod) MACRO_CONCAT(__mod_info__,s_mod)

/* Internal declarations. */

#define __MOD_DECL(s_mod) \
	extern const struct mod __MOD(s_mod)

#define __MOD_INFO_DECL(s_mod) \
	extern const struct __mod_info __MOD_INFO(s_mod) __attribute__ ((weak))

#endif /* IMPL_MOD_DECLS_H_ */
