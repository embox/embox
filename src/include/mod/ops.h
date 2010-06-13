/**
 * @file
 * @brief TODO
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_OPS_H_
#define MOD_OPS_H_

struct mod;

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
 * The main mod operation. The semantics of the operation is module-specific.
 * Mods framework does not use this method nor its return value. Please note
 * that framework also does not track the current state of the mod or its
 * dependencies, so the mod should check its state by itself.
 *
 * The caller should use #mod_invoke() function instead of directly accessing
 * the corresponding field of the #mod_ops structure.
 *
 * @param self pointer to the #mod struct.
 * @param data optional argument
 * @return operation result
 */
typedef int (*mod_invoke_t)(struct mod *self, void *data);

/**
 * Module operations.
 * TODO more docs. -- Eldar
 * @note Do not call these functions directly!
 */
struct mod_ops {
	/** (optional) Module state change operation. */
	mod_op_t enable, disable;
	/** (optional) Module main method. */
	mod_invoke_t invoke;
};

#endif /* MOD_OPS_H_ */
