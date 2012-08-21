/**
 * @file
 *
 * @brief
 *
 * @date 03.07.2012
 * @author Anton Bondarev
 */

#ifndef FRAMEWORK_MOD_OPTIONS_H_
#define FRAMEWORK_MOD_OPTIONS_H_

#include <util/macro.h>

#define __OPTION_MODULE_GET(mod,type,name) \
		OPTION_##type##_##mod##__##name

#define OPTION_MODULE_GET(mod,type,name) \
	__OPTION_MODULE_GET(mod,type,name)

#define OPTION_GET(type,name) \
	OPTION_MODULE_GET(__EMBUILD_MOD__,type,name)

#define __OPTION_MODULE_DEFINED(mod,type,name) \
	defined(OPTION_##type##_##mod##__##name)

#define OPTION_MODULE_DEFINED(mod,type,name) \
	  __OPTION_MODULE_DEFINED(mod,type,name)

#define OPTION_DEFINED(type,name) \
	OPTION_MODULE_DEFINED(__EMBUILD_MOD__,type,name)

/* Performs additional stringification, handy in string options */
#define OPTION_STRING_GET(name) \
	MACRO_STRING(OPTION_GET(STRING,name))

#endif /* FRAMEWORK_MOD_OPTIONS_H_ */
