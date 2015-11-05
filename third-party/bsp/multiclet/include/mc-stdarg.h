#ifndef MC_STDARG_H
#define MC_STDARG_H
/*
typedef struct __va_list* va_list;

#define __VA_ALIGNED_SIZE(x) ((sizeof(x) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))

#define va_start(ap, last) ((ap) = (void *)(((char *)&(last)) + __VA_ALIGNED_SIZE(last)))
#define va_end(ap) ((void*)0)

#define va_arg(ap, type) \
	( ((ap) = (va_list)((char *)(ap) + __VA_ALIGNED_SIZE(type))), \
	*(type *)(void *)((char *)(ap) - __VA_ALIGNED_SIZE(type)) )
*/

////////////////////////////////////////////////////////////////////////


typedef struct
{
 void *stack_ptr;
} __va_list;

typedef __va_list* va_list;

#define __VA_ALIGNED_SIZE(x) ((sizeof(x) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))

#define va_start(ap, last) __va_list __va_list__ = { 0 }; __va_list__.stack_ptr = (void *)(((char *)&(last)) + __VA_ALIGNED_SIZE(last)); (ap) = &__va_list__
#define va_end(ap) ((void*)0)

#define va_arg(ap, type) \
	( ((ap)->stack_ptr = (va_list)((char *)(ap)->stack_ptr + __VA_ALIGNED_SIZE(type))), \
	*(type *)(void *)((char *)(ap)->stack_ptr - __VA_ALIGNED_SIZE(type)) )
#endif
