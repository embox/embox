/**
 * @file
 * @brief EMBOX Dependency Injection core API
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_CORE_H_
#define MOD_CORE_H_

#include <stdbool.h>

#include <impl/mod/core.h>

/**
 * TODO Module info emitted by EMBuild dependency injection model generator.
 */
struct mod;

/**
 * All mods in the system are grouped into so-called packages. Packages are
 * useful only during the image build where they act as isolated mod
 * namespaces and compilation domains.
 * Package has no special run-time semantics.
 */
struct mod_package;

/**
 * Used to iterate over various mod lists.
 */
struct mod_iterator;

/**
 * Performs an operation with the module. The semantics of the operation is
 * module-specific. If the module has no operation assigned (#mod_ops structure
 * contains @c NULL pointer fields), the meaning is that module operation
 * always succeeds (as if the corresponding function returns 0).
 *
 * @param self pointer to the #mod struct.
 * @return error code
 * @retval 0 if operation succeeds
 * @retval nonzero on error
 */
typedef int (*mod_op_t)(struct mod *self);

/**
 * Module operations.
 * TODO more docs. -- Eldar
 * @note Do not call these functions directly!
 */
struct mod_ops {
	/** (optional) Module state change operation. */
	mod_op_t enable, disable;
};

/**
 * Enables the specified mod resolving its dependencies. This implies that all
 * the mods on which the given one depends will also be enabled.
 * If the mod has already been enabled then nothing special is done and this
 * function returns zero.
 *
 * @param mod the mod to enable
 * @return operation result
 * @retval 0 if the mod has been successfully enabled
 * @retval -EINVAL if the argument is @c NULL
 * @retval -EINTR if an error has occurred while enabling the mod or one of
 *         its dependencies
 */
extern int mod_enable(const struct mod *mod);

/**
 * Disables the specified mod resolving its dependencies. This implies that all
 * the mods which depend on the given one will also be disabled.
 * If the mod has not been enabled yet then nothing special is done and this
 * function returns zero.
 *
 * @param mod the mod to disable
 * @return operation result
 * @retval 0 if the mod has been successfully disabled
 * @retval -EINVAL if the argument is @c NULL
 * @retval -EINTR if an error has occurred while disabling the mod or one of
 *         its dependencies
 */
extern int mod_disable(const struct mod *mod);

/**
 * The weak version of #mod_enable().
 * Enables the specified mod if and only if all the mods on which the given one
 * depends are also enabled. If the mod has already been enabled then nothing
 * special is done and the function returns zero.
 *
 * @param mod the mod to enable
 * @return operation result
 * @retval 0 if the mod has been successfully enabled
 * @retval -EINVAL if the argument is @c NULL
 * @retval -EBUSY if the mod cannot be enabled at the moment because of
 *         unsatisfied dependencies
 * @retval -EINTR if an error has occurred while enabling the mod
 */
extern int mod_enable_nodep(const struct mod *mod);

/**
 * The weak version of #mod_disable().
 * Disables the specified mod if and only if all the mods which depend on the
 * given one are also disabled. If the mod has not been enabled yet then
 * nothing special is done and the function returns zero.
 *
 * @param mod the mod to disable
 * @return operation result
 * @retval 0 if the mod has been successfully disabled
 * @retval -EINVAL if the argument is @c NULL
 * @retval -EBUSY if the mod cannot be disabled at the moment because of
 *         unsatisfied dependencies
 * @retval -EINTR if an error has occurred while disabling the mod
 */
extern int mod_disable_nodep(const struct mod *mod);

/**
 * Tells whether the specified mod is enabled or not.
 *
 * @param mod the mod to check
 * @return the running status of the mod
 * @retval true if the mod is enabled
 * @retval false if argument is @c NULL or mod is disabled
 */
extern bool mod_is_running(const struct mod *mod);

/**
 * Gets the data associated with the specified mod (if any).
 *
 * @param mod the mod which's data to get
 * @return the mod data
 */
extern void *mod_data(const struct mod *mod);

/**
 * Gets the list of mods on which the specified one depends.
 *
 * @param mod the target mod
 * @param iterator pointer to the #mod_iterator structure instance
 * @return iterator over the list of mod dependencies
 * @retval the value passed as the @c iterator argument
 * @retval NULL if one or more arguments is @c NULL
 */
extern struct mod_iterator *mod_requires(const struct mod *mod,
		struct mod_iterator *iterator);

#define mod_foreach_requires(dep, mod) \
		__mod_foreach_requires(dep, mod)


/**
 * Gets the list of mods which depend on the specified one.
 *
 * @param mod the target mod
 * @param iterator pointer to the #mod_iterator structure instance
 * @return iterator over the list of requested mods
 * @retval the value passed as the @c iterator argument
 * @retval NULL if one or more arguments is @c NULL
 */
extern struct mod_iterator *mod_provides(const struct mod *mod,
		struct mod_iterator *iterator);

#define mod_foreach_provides(dep, mod) \
		__mod_foreach_provides(dep, mod)

/**
 * Retrieves the next mod in the iteration (if any).
 *
 * @param iterator the iterator over the list of mods
 * @return the next element in the iteration
 * @retval the next #mod instance if such exists
 * @retval NULL if argument is @c NULL or iteration has no more elements
 */
extern struct mod *mod_iterator_next(struct mod_iterator *iterator);

/**
 * Tells whether the iteration has more elements.
 *
 * @param iterator the iterator over the list of mods
 * @return the check result
 * @retval true if the iterator has more elements
 * @retval false if argument is @c NULL or iteration has no more elements
 */
extern bool mod_iterator_has_next(struct mod_iterator *iterator);

#endif /* MOD_CORE_H_ */
