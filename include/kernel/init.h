/**
 * \file init.h
 *
 * \date Nov 23, 2009
 * \author afomin
 */

#ifndef INIT_H_
#define INIT_H_

#define INIT_MODULE_LEVEL     1
#define INIT_EXPR_TESTS_LEVEL 3
#define INIT_NET_LEVEL        4
#define INIT_FS_LEVEL         4

#define INIT_MAX_LEVEL 4

typedef struct init_descriptor {
    const char *name;
	int (*init)();
	int level;
} init_descriptor_t;

#define DECLARE_INIT(name, init, level) \
    static int init(); \
    static const init_descriptor_t _descriptor##init = { name, init, level }; \
    static const init_descriptor_t *_pdescriptor##init \
		__attribute__ ((used, section(".init_handlers"))) \
		= &_descriptor##init


#endif /* INIT_H_ */
