/**
 * @file
 * @brief
 *
 * @date 07.11.2012
 * @author Anton Bulychev
 */

#ifndef ARCH_SPARC_KERNEL_SYSCALL_SYSCALLER_H_
#define ARCH_SPARC_KERNEL_SYSCALL_SYSCALLER_H_

#define __SYSCALL0(NR,type,name)  \
static inline type name(void)     \
{                                 \
long __res;	                      \
__asm__ __volatile__ (            \
		"mov %1, %%o0\n\t"        \
        "ta 0x99\n\t"             \
         "nop\n\t"                \
        "mov %%o0, %0\n\t"        \
                                  \
        :"=r" (__res)             \
        :"r" (NR)                 \
	);                            \
return __res;                     \
}

#define __SYSCALL1(NR,type,name,type1,arg1)  \
static inline type name(type1 arg1)          \
{                                            \
long __res;	                                 \
__asm__ __volatile__ (                       \
		"mov %1, %%o0\n\t"                   \
		"mov %2, %%o1\n\t"                   \
        "ta 0x99\n\t"                        \
         "nop\n\t"                           \
        "mov %%o0, %0\n\t"                   \
                                             \
        :"=r" (__res)                        \
        :"r" (NR),                           \
         "r" ((long)(arg1))                  \
	);                                       \
return __res;                                \
}
#define __SYSCALL2(NR,type,name,type1,arg1,type2,arg2)  \
static inline type name(type1 arg1,type2 arg2)          \
{                                                       \
long __res;	                                            \
__asm__ __volatile__ (                                  \
		"mov %1, %%o0\n\t"                              \
		"mov %2, %%o1\n\t"                              \
		"mov %3, %%o2\n\t"                              \
        "ta 0x99\n\t"                                   \
         "nop\n\t"                                      \
        "mov %%o0, %0\n\t"                              \
                                                        \
        :"=r" (__res)                                   \
        :"r" (NR),                                      \
         "r" ((long)(arg1)),                            \
         "r" ((long)(arg2))                             \
	);                                                  \
return __res;                                           \
}

#define __SYSCALL3(NR,type,name,type1,arg1,type2,arg2,type3,arg3)  \
static inline type name(type1 arg1,type2 arg2,type3 arg3)          \
{                                                                  \
long __res;	                                                       \
__asm__ __volatile__ (                                             \
		"mov %1, %%o0\n\t"                                         \
		"mov %2, %%o1\n\t"                                         \
		"mov %3, %%o2\n\t"                                         \
		"mov %4, %%o3\n\t"                                         \
        "ta 0x99\n\t"                                              \
         "nop\n\t"                                                 \
        "mov %%o0, %0\n\t"                                         \
                                                                   \
        :"=r" (__res)                                              \
        :"r" (NR),                                                 \
         "r" ((long)(arg1)),                                       \
         "r" ((long)(arg2)),                                       \
         "r" ((long)(arg3))                                        \
	);                                                             \
return __res;                                                      \
}

#define __SYSCALL4(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)   \
static inline type name(type1 arg1,type2 arg2,type3 arg3,type4 arg4)           \
{                                                                              \
long __res;	                                                                   \
__asm__ __volatile__ (                                                         \
		"mov %1, %%o0\n\t"                                                     \
		"mov %2, %%o1\n\t"                                                     \
		"mov %3, %%o2\n\t"                                                     \
		"mov %4, %%o3\n\t"                                                     \
		"mov %5, %%o4\n\t"                                                     \
        "ta 0x99\n\t"                                                          \
         "nop\n\t"                                                             \
        "mov %%o0, %0\n\t"                                                     \
                                                                               \
        :"=r" (__res)                                                          \
        :"r" (NR),                                                             \
         "r" ((long)(arg1)),                                                   \
         "r" ((long)(arg2)),                                                   \
         "r" ((long)(arg3)),                                                   \
         "r" ((long)(arg4))                                                    \
	);                                                                         \
return __res;                                                                  \
}

#define __SYSCALL5(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,   \
          type5,arg5)                                                          \
static inline type name(type1 arg1,type2 arg2,type3 arg3,type4 arg4,           \
          type5 arg5)                                                          \
{                                                                              \
long __res;	                                                                   \
__asm__ __volatile__ (                                                         \
		"mov %1, %%o0;"                                                        \
		"mov %2, %%o1;"                                                        \
		"mov %3, %%o2;"                                                        \
		"mov %4, %%o3;"                                                        \
		"mov %5, %%o4;"                                                        \
		"mov %6, %%o5;"                                                        \
        "ta 0x99;"                                                             \
         "nop;"                                                                \
        "mov %%o0, %0\n\t"                                                     \
                                                                               \
        :"=r" (__res)                                                          \
        :"r" (NR),                                                             \
         "r" ((long)(arg1)),                                                   \
         "r" ((long)(arg2)),                                                   \
         "r" ((long)(arg3)),                                                   \
         "r" ((long)(arg4)),                                                   \
         "r" ((long)(arg5))                                                    \
	);                                                                         \
return __res;                                                                  \
}

#endif /* ARCH_SPARC_KERNEL_SYSCALL_SYSCALLER_H_ */
