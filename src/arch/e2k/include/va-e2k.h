/* Internals of varargs for e2k */
#ifndef VA_E2K_H
#define VA_E2K_H

#if (!defined __STDARG_H) && (!defined __VARARGS_H)
#error "do not include va-e2k.h directly"
#endif

#define __PAR_SIZE              8

#define __arg_size(x)           ((sizeof(x)+__PAR_SIZE-1) & ~(__PAR_SIZE-1))

#ifdef __VARARGS_H

/* Traditional interface */

/* __builtin_va_alist is magically recognized as synonim for ...
   for old-style fucntions. */
#define va_alist                __va_alist,__builtin_va_alist
#define va_dcl                  va_list __va_alist; int __builtin_va_alist;

#define va_start(ap)            ((ap) = (va_list)&__va_alist)

#else /* __VARARGS_H */

/* Standard interface */

#define va_start(ap, parmN)     \
     ((ap) = (va_list)((char *)(&parmN)+__arg_size(parmN)))

#endif /* __VARARGS_H */

#define va_arg(ap, type)        \
        ( ( ap = (char*)ap +    \
            ((~((int)((char*)ap-(char*)0))+1) & (((__PAR_SIZE<<1)-1)>>(sizeof(type) <= __PAR_SIZE))) \
            ),\
        (*(type *)((char*)( ap = (char*)ap + __arg_size(type)) - __arg_size(type))) )

#define va_end(ap)              ((void)0)

#define va_copy(dest, src)      ((dest) = (src))

#endif /* VA_E2K_H */
