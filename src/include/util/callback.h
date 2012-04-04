/**
 * @file
 * @brief Callback body macros
 *
 * @date 07.07.11
 * @author Anton Kozlov
 */

#ifndef UTIL_CALLBACK_H_
#define UTIL_CALLBACK_H_

#include <framework/service/types.h>

#define __CALLBACK_NAME(name) __##name##_callback

#define CALLBACK_INIT(type, name) \
		type __CALLBACK_NAME(name)

#define CALLBACK_INIT_DEF(type, name, def) \
		type __CALLBACK_NAME(name) = def

#define CALLBACK_DECLARE(type, name) \
		extern type __CALLBACK_NAME(name)

#define CALLBACK_REG(name, callback) \
		__CALLBACK_NAME(name) = callback

#define CALLBACK(name) __CALLBACK_NAME(name)

#endif /* UTIL_CALLBACK_H_ */
