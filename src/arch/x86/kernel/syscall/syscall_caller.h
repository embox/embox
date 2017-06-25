/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#ifndef ARCH_X86_KERNEL_SYSCALL_SYSCALLER_H_
#define ARCH_X86_KERNEL_SYSCALL_SYSCALLER_H_

#define __SYSCALL0(NR,type,name)  \
static inline type name(void)     \
{                                 \
long __res;                       \
__asm__ volatile ("int $0x80"     \
        : "=a" (__res)            \
        : "0"  (NR));             \
return __res;                     \
}

#define __SYSCALL1(NR,type,name,type1,arg1) \
static inline type name(type1 arg1)         \
{                                           \
long __res;                                 \
__asm__ volatile ("int $0x80"               \
        : "=a" (__res)                      \
        : "0"  (NR),                        \
          "b"  ((long)(arg1)));             \
return __res;                               \
}

#define __SYSCALL2(NR,type,name,type1,arg1,type2,arg2)  \
static inline type name(type1 arg1,type2 arg2)          \
{                                                       \
long __res;                                             \
__asm__ volatile ("int $0x80"                           \
        : "=a" (__res)                                  \
        : "0"  (NR),                                    \
          "b"  ((long)(arg1)),                          \
          "c"  ((long)(arg2)));                         \
return __res;                                           \
}

#define __SYSCALL3(NR,type,name,type1,arg1,type2,arg2,type3,arg3)  \
static inline type name(type1 arg1,type2 arg2,type3 arg3)          \
{                                                                  \
long __res;                                                        \
__asm__ volatile ("int $0x80"                                      \
        : "=a" (__res)                                             \
        : "0"  (NR),                                               \
          "b"  ((long)(arg1)),                                     \
          "c"  ((long)(arg2)),                                     \
          "d"  ((long)(arg3)));                                    \
return __res;                                                      \
}

#define __SYSCALL4(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)  \
static inline type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)       \
{                                                                             \
long __res;                                                                   \
__asm__ volatile ("int $0x80"                                                 \
        : "=a" (__res)                                                        \
        : "0"  (NR),                                                          \
          "b"  ((long)(arg1)),                                                \
          "c"  ((long)(arg2)),                                                \
          "d"  ((long)(arg3)),                                                \
          "S"  ((long)(arg4)));                                               \
return __res;                                                                 \
}

#define __SYSCALL5(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,   \
          type5,arg5)                                                          \
static inline type name(type1 arg1,type2 arg2,type3 arg3,type4 arg4,           \
          type5 arg5)                                                          \
{                                                                              \
long __res;                                                                    \
__asm__ volatile ("int $0x80"                                                  \
        : "=a" (__res)                                                         \
        : "0"  (NR),                                                           \
          "b"  ((long)(arg1)),                                                 \
          "c"  ((long)(arg2)),                                                 \
          "d"  ((long)(arg3)),                                                 \
          "S"  ((long)(arg4)),                                                 \
          "D"  ((long)(arg5)));                                                \
return __res;                                                                  \
}

/* Not used */

#define __SYSCALL6(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,   \
          type5,arg5,type6,arg6)                                               \
static inline type name(type1 arg1,type2 arg2,type3 arg3,type4 arg4,           \
          type5 arg5,type6 arg6)                                               \
{                                                                              \
long __res;                                                                    \
__asm__ volatile ("push %%ebp;"                                                \
				  "movl %%eax,%%ebp;"                                          \
                  "movl %1,%%eax;"                                             \
                  "int $0x80;"                                                 \
                  "pop %%ebp"                                                  \
		                                                                       \
        : "=a" (__res)                                                         \
        : "i"  (NR),                                                           \
          "b"  ((long)(arg1)),                                                 \
          "c"  ((long)(arg2)),                                                 \
          "d"  ((long)(arg3)),                                                 \
          "S"  ((long)(arg4)),                                                 \
          "D"  ((long)(arg5)),                                                 \
          ""   ((long)(arg6)));                                                \
return __res;                                                                  \
}

#endif /* ARCH_X86_KERNEL_SYSCALL_CALLER_H_ */
