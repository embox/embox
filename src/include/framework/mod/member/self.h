/**
 * @file
 * @brief Self mod bindings for #mod_member.
 * @details
 *   Member bindings are used to initialize (finalize) each of the module's
 *   @link #mod_member members @endlink before enabling (after disabling) the
 *   whole module.
 *
 * @note
 *   Do not include from any source which is not a part of some mod.
 *
 * @date Jul 05, 2011
 * @author Alexandr Kalmuk
 *          - Module members binding
 * @author Eldar Abusalimov
 *          - Documentation
 */

#ifndef FRAMEWORK_MOD_MEMBER_SELF_H_
#define FRAMEWORK_MOD_MEMBER_SELF_H_

#include <framework/mod/member/ops.h>

#include __impl_x(framework/mod/member/self_impl.h)

/**
 * Binds the specified data and operations to the mod member.
 *
 * @param member_ops
 *   Pointer to the #mod_member_ops structure (if any).
 * @param member_data
 *   Pointer to the member specific data (if any).
 */
#define MOD_MEMBER_BIND(member_ops, member_data) \
	  __MOD_MEMBER_BIND(member_ops, member_data)

#endif /* FRAMEWORK_MOD_MEMBER_SELF_H_ */
