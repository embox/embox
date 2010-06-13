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
 * Does the same as #MOD_INFO_DEF() and also tags the current mod with the
 * specified tag.
 *
 * @param s_mod symbol name of the module
 * @param mod_data pointer to the module specific data (if any)
 * @param mod_ops pointer to the #mod_ops structure (if any)
 * @param s_tag symbol name of the tag defined with #MOD_TAG_DEF() macro
 */
#define MOD_INFO_TAGGED_DEF(s_mod, mod_data, mod_ops, s_tag) \
		__MOD_INFO_TAGGED_DEF(s_mod, mod_data, mod_ops, s_tag)

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
 * Associates the specified mod data and operations with the current mod.
 *
 * @param _mod_data pointer to the module specific data (if any)
 * @param _mod_ops pointer to the #mod_ops structure (if any)
 *
 * @see MOD_INFO_DEF()
 */
#define MOD_SELF_INFO_DEF(_mod_data, _mod_ops) \
		__MOD_SELF_INFO_DEF(_mod_data, _mod_ops)

/**
 * Defines the mod interface as #MOD_SELF_INFO_DEF() does and also tags the
 * current mod with the specified tag.
 *
 * @param _mod_data pointer to the module specific data (if any)
 * @param _mod_ops pointer to the #mod_ops structure (if any)
 * @param s_tag symbol name of the tag defined with #MOD_TAG_DEF() macro
 *
 * @see MOD_INFO_TAGGED_DEF()
 */
#define MOD_SELF_INFO_TAGGED_DEF(_mod_data, _mod_ops, s_tag) \
		__MOD_SELF_INFO_TAGGED_DEF(_mod_data, _mod_ops, s_tag)


#endif /* MOD_BIND_H_ */
