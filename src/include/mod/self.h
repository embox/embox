/**
 * @file
 * @brief The self mod is one that is associated with the current compilation
 *        unit.
 *
 * @note Do not include from any source which is not part of some mod.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_SELF_H_
#define MOD_SELF_H_

#include __impl(mod/self.h)

/** The #mod structure corresponding to the self mod. */
extern const struct mod mod_self;

/**
 * Binds the specified mod data and operations to the self mod.
 *
 * @param mod_data pointer to the module specific data (if any)
 * @param mod_ops pointer to the #mod_ops structure (if any)
 */
#define MOD_SELF_BIND(mod_data, mod_ops) \
		__MOD_SELF_BIND(mod_data, mod_ops)

#endif /* MOD_SELF_H_ */
