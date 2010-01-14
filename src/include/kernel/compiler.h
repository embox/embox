/**
 * @file
 * @brief Describes compiler specific features
 * @date 13.01.2010
 * @author Anton Bondarev
 */

#ifndef COMPILER_H_
#define COMPILER_H_

/* In linux it defined as "noderef". It means that pointer cannot be
 * dereferenced. And use in sys_calls because this variable must be copied by
 * opy_from_user and copy_to_user functions
 */
#define __user

#endif /* COMPILER_H_ */
