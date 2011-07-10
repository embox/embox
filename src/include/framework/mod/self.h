/**
 * @file
 * @brief Self mod bindings for #mod_info.
 * @details
 *   Self mod is the one that is associated with the current compilation unit.
 *   Bindings associate application-specific operations and data with the self
 *   mod. Such information is then used to properly initialize and finalize
 *   the @link #mod_info module @endlink.
 *
 * @note
 *   Do not include from any source which is not a part of some mod.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 *          - Initial implementation for mod_info
 */

#ifndef FRAMEWORK_MOD_SELF_H_
#define FRAMEWORK_MOD_SELF_H_

#include <framework/mod/ops.h>

#include __impl_x(framework/mod/self_impl.h)

/** The #mod structure corresponding to the self mod. */
extern const struct mod mod_self;

/**
 * Binds the specified mod data and operations to the self mod.
 *
 * @param mod_ops
 *   Pointer to the #mod_ops structure (if any).
 * @param mod_data
 *   Pointer to the module specific data (if any).
 */
#define MOD_INFO_BIND(mod_ops, mod_data) \
	  __MOD_INFO_BIND(mod_ops, mod_data)

#endif /* FRAMEWORK_MOD_SELF_H_ */
