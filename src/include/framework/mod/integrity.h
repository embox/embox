/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.03.2017
 */

#ifndef FRAMEWORK_MOD_INTEGRITY_H_
#define FRAMEWORK_MOD_INTEGRITY_H_

struct mod;

/**
 * Check module for integrity.
 *
 * @param mod
 *
 * @retval -ENOTSUP
 *   Module integrity check not supported
 * @retval 0
 *   Module is OK
 * @retval 1
 *   Module is broken
 */
extern int mod_integrity_check(const struct mod *mod);

#endif /* FRAMEWORK_MOD_INTEGRITY_H_ */
