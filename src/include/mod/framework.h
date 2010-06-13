/**
 * @file
 * @brief TODO
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_FRAMEWORK_H_
#define MOD_FRAMEWORK_H_

#include <stdbool.h>

#include <impl/mod/framework.h>

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
 * Invokes the module if it has provided the corresponding operation.
 * Please note that framework does not track the current state of the mod or
 * its dependencies, this means that the mod will be invoked even if it is not
 * enabled now.
 *
 * @param mod the mod on which to call @link mod_ops#invoke @endlink method
 * @param data optional argument to pass to the @c invoke method
 * @return invocation result
 * @retval -EINVAL if the @c mod argument is @c NULL
 * @retval -ENOTSUP if the mod does not support invoke method
 */
extern int mod_invoke(const struct mod *mod, void *data);

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

#if 0
/* TODO there is no way to implement these functions at now. -- Eldar */

/**
 * Sets the mod-specific data.
 *
 * @param mod the mod which's data to get
 * @param data the data to associate with the mod
 */
extern void mod_data_set(const struct mod *mod, void *data);

/**
 * Gets the #mod_ops of the specified mod.
 *
 * @param mod the mod which's ops to get
 * @return the mod operations structure
 */
extern struct mod_ops *mod_ops_get(const struct mod *mod);

/**
 * Sets the mod-specific data.
 *
 * @param mod the mod which's data to get
 * @param ops the data to associate with the mod
 */
extern void mod_ops_set(const struct mod *mod, struct mod_ops *ops);
#endif

#endif /* MOD_FRAMEWORK_H_ */
