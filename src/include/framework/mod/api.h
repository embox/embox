/**
 * @file
 * @brief Embox Dependency Injection core API.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_API_H_
#define FRAMEWORK_MOD_API_H_

#include <stdbool.h>

#include __impl_x(framework/mod/api_impl.h)

/**
 * TODO Module info emitted by EMBuild dependency injection model generator.
 */
struct mod;

/**
 * All mods in the system are grouped into so-called packages. Packages are
 * useful only during the image build where they act as isolated mod
 * namespaces and compilation domains.
 *
 * Package has no special run-time semantics.
 */
struct mod_package;

/**
 * Performs an operation with the module. The semantics of the operation is
 * module-specific. If the module has no operation assigned (#mod_ops structure
 * contains @c NULL pointer fields), the meaning is that module operation
 * always succeeds (as if the corresponding function returns 0).
 *
 * @param self
 *   Pointer to the #mod struct.
 * @return
 *   Error code.
 * @retval 0
 *   If the operation succeeded.
 * @retval nonzero
 *   On error.
 */
typedef int (*mod_op_t)(struct mod *self);

/**
 * Module operations.
 * TODO more docs. -- Eldar
 * @note
 *   Do not call these functions directly!
 */
struct mod_ops {
	/** (optional) Module state change operation. */
	mod_op_t enable, disable;
};

/**
 * Enables the specified #mod resolving its dependencies. This implies that all
 * the mods on which the given one depends will also be enabled.
 * If the mod has already been enabled then nothing special is done and this
 * function returns zero.
 *
 * @param mod
 *   The mod to enable.
 * @return
 *   Operation result.
 * @retval 0
 *   If the mod has been successfully enabled.
 * @retval -EINVAL
 *   If the argument is @c NULL.
 * @retval -EINTR
 *   If an error has occurred while enabling the mod or one of its
 *   dependencies.
 */
extern int mod_enable(const struct mod *mod);

/**
 * Disables the specified #mod resolving its dependencies. This implies that
 * all the mods which depend on the given one will also be disabled.
 * If the mod has not been enabled yet then nothing special is done and this
 * function returns zero.
 *
 * @param mod
 *   The mod to disable
 * @return
 *   Operation result.
 * @retval 0
 *   If the mod has been successfully disabled.
 * @retval -EINVAL
 *   If the argument is @c NULL.
 * @retval -EINTR
 *   If an error has occurred while disabling the mod or one of its
 *   dependencies.
 */
extern int mod_disable(const struct mod *mod);

/**
 * The weak version of #mod_enable().
 * Enables the specified #mod if and only if all the mods on which the given
 * one depends are also enabled. If the mod has already been enabled then
 * nothing special is done and the function returns zero.
 *
 * @param mod
 *   The mod to enable.
 * @return
 *   Operation result.
 * @retval 0
 *   If the mod has been successfully enabled.
 * @retval -EINVAL
 *   If the argument is @c NULL.
 * @retval -EBUSY
 *   If the mod cannot be enabled at the moment because of unsatisfied
 *   dependencies.
 * @retval -EINTR
 *   If an error has occurred while enabling the mod.
 */
extern int mod_enable_nodep(const struct mod *mod);

/**
 * The weak version of #mod_disable().
 * Disables the specified mod if and only if all the mods which depend on the
 * given one are also disabled. If the mod has not been enabled yet then
 * nothing special is done and the function returns zero.
 *
 * @param mod
 *   The mod to disable.
 * @return
 *   Operation result.
 * @retval 0
 *   If the mod has been successfully disabled.
 * @retval -EINVAL
 *   If the argument is @c NULL.
 * @retval -EBUSY
 *   If the mod cannot be disabled at the moment because of unsatisfied
 *   dependencies.
 * @retval -EINTR
 *   If an error has occurred while disabling the mod.
 */
extern int mod_disable_nodep(const struct mod *mod);

/**
 * Tells whether the specified mod is enabled or not.
 *
 * @param mod
 *   The mod to check.
 * @return
 *   The running status of the mod.
 * @retval true
 *   If the mod is enabled.
 * @retval false
 *   If argument is @c NULL or mod is disabled.
 */
extern bool mod_is_running(const struct mod *mod);

/**
 * Gets the data associated with the specified mod (if any).
 *
 * @param mod
 *   The mod which's data to get.
 * @return
 *   The mod data (or @c NULL of no data has been attached to the mod).
 */
extern void *mod_data(const struct mod *mod);

/**
 * Iterates over a list of all mods registered in the system
 * either running or not.
 *
 * @param mod
 *   Iteration variable which takes a value of each mod.
 */
#define mod_foreach(mod) \
	  __mod_foreach(mod)

/**
 * Iterates over a list of mods on which the specified one depends.
 *
 * @param dep
 *   Iteration variable which takes a value of each element of the mod
 *   dependencies list.
 * @param mod
 *   The target mod.
 */
#define mod_foreach_requires(dep, mod) \
	  __mod_foreach_requires(dep, mod)

/**
 * Iterates over a list of mods which depend on the specified one.
 *
 * @param dep
 *   Iteration variable which takes a value of each mod depending on the
 *   target mod.
 * @param mod
 *   The target mod.
 */
#define mod_foreach_provides(dep, mod) \
	  __mod_foreach_provides(dep, mod)

#endif /* FRAMEWORK_MOD_API_H_ */
