/**
 * \file module.h
 * \date Jul 9, 2009
 * \author anton
 * \details
 */
#ifndef MODULE_H_
#define MODULE_H_

#include "kernel/init.h"

#define DECLARE_MODULE(name, init) \
	DECLARE_INIT(name, init, INIT_MODULE_LEVEL)

typedef int (*INIT_FUNCTION_HANDLER)(void);

#endif /* MODULE_H_ */
