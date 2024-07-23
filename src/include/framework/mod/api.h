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
#include <sys/cdefs.h>

#include <framework/mod/ops.h>
#include <framework/mod/runlevel.h>
#include <framework/mod/types.h>
#include <lib/libds/array.h>
#include <util/macro.h>

/**
 * Iterates over a list of mods on which the specified one depends.
 *
 * @param dep
 *   Iteration variable which takes a value of each element of the mod
 *   dependencies list.
 * @param mod
 *   The target mod.
 */
#define mod_foreach_depends(dep, mod) __mod_foreach_depends(dep, mod)

#if USE_MOD_DEPENDS
#define __mod_foreach_depends(dep, mod) \
	array_spread_nullterm_foreach(dep, mod->depends)
#else

#define __mod_foreach_depends(dep, mod) while (0)
#endif

/**
 * Iterates over a list of mods which depend on the specified one.
 *
 * @param dep
 *   Iteration variable which takes a value of each mod depending on the
 *   target mod.
 * @param mod
 *   The target mod.
 */
#define mod_foreach_provides(dep, mod) __mod_foreach_provides(dep, mod)

#if USE_MOD_DEPENDS
#define __mod_foreach_provides(dep, mod) \
	array_spread_nullterm_foreach(dep, mod->provides)
#else
#define __mod_foreach_provides(dep, mod) while (0)
#endif

/**
 * Iterates over a list of runtime modules either running or not.
 *
 * @param mod
 *   Iteration variable which takes a value of each mod.
 */
#define mod_foreach(mod) __mod_foreach(mod, MACRO_GUARD(__i))

#define __mod_foreach(mod, _i)                      \
	for (int _i = 0; _i < RUNLEVEL_NRS_TOTAL; _i++) \
	runlevel_mod_foreach(mod, _i)

__BEGIN_DECLS

/**
 * Enables the specified #mod resolving its dependencies. This implies that all
 * the mods on which the given one depends will also be enabled.
 * If the mod has already been enabled then nothing special is done and this
 * function returns zero.
 * Althoug, when module has been not enabled, but it depends on modules that are in
 * enabling state, it produce error since it may be caused by cyclic dependecies.
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
 * @brief Enables specified #mod resolving it's dependencies. If #recursive_safe
 * flag is set, founded possible cyclic dependendicies ignored and not generating
 * errors
 *
 * @param mod
 *   The mod to enable
 * @param recursive_safe
 *   Cyclic detection flag. When set cyclic detection not generating error.
 *
 * @return
 *   Same as #mod_enable
 */
extern int mod_enable_rec_safe(const struct mod *mod, bool recursive_safe);

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
 * Search for a module with a given FQN (fully.qualified.name)
 * @param fqn
 *   Module name, including packages.
 * @return
 *   Found module, if any, NULL otherwise.
 */
extern const struct mod *mod_lookup(const char *fqn);

static inline const char *mod_name(const struct mod *mod) {
	return mod->mod_name;
}

static inline const char *mod_pkg_name(const struct mod *mod) {
	return mod->pkg_name;
}

static inline const struct mod_label *mod_label(const struct mod *mod) {
	return mod->label;
}

__END_DECLS

#endif /* FRAMEWORK_MOD_API_H_ */
