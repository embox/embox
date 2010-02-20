/**
 * @file
 *
 * @date 09.07.2009
 * @author Anton Bondarev
 */
#ifndef MODULE_H_
#define MODULE_H_

#include <kernel/init.h>

#define DECLARE_MODULE(name, init) \
	DECLARE_INIT(name, init, INIT_MODULE_LEVEL)

typedef int (*INIT_FUNCTION_HANDLER)(void);

#endif /* MODULE_H_ */
