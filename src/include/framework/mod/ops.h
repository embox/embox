/**
 * @file
 * @brief TODO
 *
 * @date Jul 10, 2011
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_OPS_H_
#define FRAMEWORK_MOD_OPS_H_

// XXX struct mod definition. -- Eldar
#include __impl_x(framework/mod/types.h)

struct mod;
struct mod_info;

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
 * Mods framework manages each mod through this special interface.
 */
struct mod_info {
	const struct mod     *mod;  /**< Corresponding module. */
	const struct mod_ops *ops;  /**< (optional) Available operations. */
	void                 *data; /**< (optional) Module specific data. */
};

/**
 * Gets the data associated with the specified mod (if any).
 *
 * @param mod
 *   The mod which's data to get.
 * @return
 *   The mod data (or @c NULL of no data has been attached to the mod).
 */
static inline void *mod_data(const struct mod *mod) {
	return (mod && mod->info) ? mod->info->data : NULL;
}

#endif /* FRAMEWORK_MOD_API_H_ */
