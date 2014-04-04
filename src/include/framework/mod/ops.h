/**
 * @file
 * @brief TODO
 *
 * @date Jul 10, 2011
 * @author Eldar Abusalimov
 * @author Alexandr Kalmuk
 *          - Modulemember ops
 */

#ifndef FRAMEWORK_MOD_OPS_H_
#define FRAMEWORK_MOD_OPS_H_

struct mod;
struct mod_member;

/**
 * Performs an operation with the module. The semantics of the operation is
 * module-specific. If the module has no operation assigned (#mod_ops structure
 * contains @c NULL pointer fields), the meaning is that module operation
 * always succeeds (as if the corresponding function returns 0).
 *
 * @param self
 *   Pointer to the #mod_info struct.
 * @return
 *   Error code.
 * @retval 0
 *   If the operation succeeded.
 * @retval nonzero
 *   On error.
 */
typedef int (*mod_op_t)(const struct mod *self);

/**
 * Performs an injection of the specified member. The semantics of the
 * operation is application-specific.
 *
 * @param member
 *   Pointer to the member being injected.
 * @return
 *   Error code.
 * @retval 0
 *   If the operation succeeded.
 * @retval nonzero
 *   On error.
 */
typedef int (*mod_member_op_t)(const struct mod_member *member);

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
 * Operations for initializing and finalizing injected members of the module.
 */
struct mod_member_ops {
	/** (optional) Member operation. */
	mod_member_op_t init, fini;
};

/**
 * Mods framework manages each mod through this special interface.
 */
struct mod_info {
	const struct mod     *mod;  /**< Corresponding module. */
	const struct mod_ops *ops;  /**< (optional) Available operations. */
	void                 *data; /**< (optional) Module specific data. */
};

struct mod_member {
	const struct mod_member_ops *ops;  /**< (optional) Available operations. */
	void                        *data; /**< (optional) Member specific data. */
};

#endif /* FRAMEWORK_MOD_API_H_ */
