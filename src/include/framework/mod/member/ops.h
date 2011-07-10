/**
 * @file
 * @brief TODO
 *
 * @date Jul 10, 2011
 * @author Alexandr Kalmuk
 *          - Initial implementation
 * @author Eldar Abusalimov
 *          - Separating from binding code
 */

#ifndef FRAMEWORK_MOD_MEMBER_OPS_H_
#define FRAMEWORK_MOD_MEMBER_OPS_H_

struct mod;
struct mod_member;

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
typedef int (*mod_member_op_t)(struct mod_member *member);

/**
 * Operations for initializing and finalizing injected members of the module.
 */
struct mod_member_ops {
	/** (optional) Member operation. */
	mod_member_op_t init, fini;
};

struct mod_member {
	const struct mod            *mod;  /**< Corresponding module. */
	const struct mod_member_ops *ops;  /**< (optional) Available operations. */
	void                        *data; /**< (optional) Member specific data. */
};

#endif /* FRAMEWORK_MOD_OPS_H_ */
