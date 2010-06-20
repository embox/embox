/**
 * @file
 * @brief EMBOX dependency injection framework.
 *
 * @date 22.02.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_BIND_H_
#define MOD_BIND_H_

#include <impl/mod/bind.h>

/**
 * Pointer to the #mod structure defined with #MOD_DEF() macro.
 *
 * @param s_mod the mod variable name used at definition time.
 */
#define MOD_PTR(s_mod) \
		__MOD_PTR(s_mod)

/**
 * Associates data and operations with the mod specified by @c s_mod argument.
 *
 * @param s_mod symbol name of the module
 * @param mod_data pointer to the module specific data (if any)
 * @param mod_ops pointer to the #mod_ops structure (if any)
 */
#define MOD_INFO_DEF(s_mod, mod_data, mod_ops) \
		__MOD_INFO_DEF(s_mod, mod_data, mod_ops)

/**
 * Pointer to the #mod structure of the module associated with current
 * compilation unit.
 *
 * @see MOD_PTR()
 */
#define MOD_SELF_PTR  __MOD_SELF_PTR

/**
 * String containing the name of the current module.
 */
#define MOD_SELF_NAME __MOD_SELF_NAME

/**
 * Binds the specified mod data and operations to the mod associated with the
 * current compilation unit.
 *
 * @param mod_data pointer to the module specific data (if any)
 * @param mod_ops pointer to the #mod_ops structure (if any)
 */
#define MOD_BIND(mod_data, mod_ops) \
		__MOD_BIND(mod_data, mod_ops)

#endif /* MOD_BIND_H_ */
