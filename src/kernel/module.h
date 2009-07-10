/**
 * \file module.h
 * \date Jul 9, 2009
 * \author anton
 * \details
 */
#ifndef MODULE_H_
#define MODULE_H_

typedef struct _MODULE_HANDLER {
	const char *name;
	int (*init)();
} MODULE_DESCRIPTOR;

#define REGISTER_MODULE(descr) static void _register_module(){ \
    __asm__( \
            ".section .modules_handlers\n\t" \
            ".word %0\n\t" \
            ".text\n" \
            : :"i"(&descr)); \
            }

#define DECLARE_MODULE(name, init) \
	static int init(); \
    static const MODULE_DESCRIPTOR _descriptor = { name, init }; \
    REGISTER_MODULE(_descriptor);

#endif /* MODULE_H_ */
