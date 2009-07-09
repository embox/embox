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
}MODULE_HANDLER;

#define REGISTER_MODULE_HANDLER(handler) \
    static void register_module(){ \
    __asm__( \
            ".section .modules_handlers\n\t" \
            ".word %0\n\t" \
            ".text\n" \
            : :"i"(handler)); \
            }

#define REGISTER_MODULE(name, module_init_func) \
    static MODULE_HANDLER module_handler = {name, module_init_func};\
    REGISTER_MODULE_HANDLER(&module_handler);



#endif /* MODULE_H_ */
