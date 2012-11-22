/**
 * @file
 *
 * @date Nov 8, 2012
 * @author: Anton Bondarev
 */

#ifndef MIPS_ARCH_SYSCALL_CALLER_H_
#define MIPS_ARCH_SYSCALL_CALLER_H_

#define __SYSCALL0(NR,type,name)  \
static inline type name(void)     \
{                                 \
long __res;                       \
__asm__ volatile (                \
		  "li $v0, %1\n\t"        \
          "syscall\n\t"           \
          "move %0, $v0"          \
                                  \
        : "=r" (__res)            \
        : "I"  (NR));             \
return __res;                     \
}

#define __SYSCALL1(NR,type,name,type1,arg1) \
static inline type name(type1 arg1)         \
{                                           \
long __res;                                 \
__asm__ volatile (                          \
          "move $a0, %2\n\t"                \
		  "li $v0, %1\n\t"                  \
          "syscall\n\t"                     \
          "move %0, $v0"                    \
                                            \
        : "=r" (__res)                      \
        : "I"  (NR),                        \
          "r"  ((long)(arg1)));             \
return __res;                               \
}

#define __SYSCALL2(NR,type,name,type1,arg1,type2,arg2)  \
static inline type name(type1 arg1,type2 arg2)          \
{                                                       \
long __res;                                             \
__asm__ volatile (                                      \
          "move $a1, %3\n\t"                            \
          "move $a0, %2\n\t"                            \
          "li $v0, %1\n\t"                              \
          "syscall\n\t"                                 \
          "move %0, $v0"                                \
                                                        \
        : "=r" (__res)                                  \
        : "I"  (NR),                                    \
          "r"  ((long)(arg1)),                          \
          "r"  ((long)(arg2)));                         \
return __res;                                           \
}

#define __SYSCALL3(NR,type,name,type1,arg1,type2,arg2,type3,arg3)  \
static inline type name(type1 arg1,type2 arg2,type3 arg3)          \
{                                                                  \
long __res;                                                        \
__asm__ volatile (                                                 \
          "move $a2, %4\n\t"                                       \
          "move $a1, %3\n\t"                                       \
          "move $a0, %2\n\t"                                       \
		  "li $v0, %1\n\t"                                         \
          "syscall\n\t"                                            \
          "move %0, $v0"                                           \
                                                                   \
        : "=r" (__res)                                             \
        : "I"  (NR),                                               \
          "r"  ((long)(arg1)),                                     \
          "r"  ((long)(arg2)),                                     \
          "r"  ((long)(arg3)));                                    \
return __res;                                                      \
}

#define __SYSCALL4(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)  \
static inline type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)       \
{                                                                             \
long __res;                                                                   \
__asm__ volatile (                                                            \
          "move $a3, %5\n\t"                                                  \
          "move $a2, %4\n\t"                                                  \
          "move $a1, %3\n\t"                                                  \
          "move $a0, %2\n\t"                                                  \
		  "li $v0, %1\n\t"                                                    \
          "syscall\n\t"                                                       \
          "move %0, $v0"                                                      \
                                                                              \
        : "=r" (__res)                                                        \
        : "I"  (NR),                                                          \
          "r"  ((long)(arg1)),                                                \
          "r"  ((long)(arg2)),                                                \
          "r"  ((long)(arg3)),                                                \
          "r"  ((long)(arg4)));                                               \
return __res;                                                                 \
}

#define __SYSCALL5(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,   \
          type5,arg5)                                                          \
static inline type name(type1 arg1,type2 arg2,type3 arg3,type4 arg4,           \
          type5 arg5)                                                          \
{                                                                              \
long __res;                                                                    \
__asm__ volatile (                                                             \
          "move $s0, %6\n\t"                                                   \
          "move $a3, %5\n\t"                                                   \
          "move $a2, %4\n\t"                                                   \
          "move $a1, %3\n\t"                                                   \
          "move $a0, %2\n\t"                                                   \
		  "li $v0, %1\n\t"                                                     \
          "syscall\n\t"                                                        \
          "move %0, $v0"                                                       \
                                                                               \
        : "=r" (__res)                                                         \
        : "I"  (NR),                                                           \
          "r"  ((long)(arg1)),                                                 \
          "r"  ((long)(arg2)),                                                 \
          "r"  ((long)(arg3)),                                                 \
          "r"  ((long)(arg4)),                                                 \
          "r"  ((long)(arg5)));                                                \
return __res;                                                                  \
}

#endif /* MIPS_ARCH_SYSCALL_CALLER_H_ */
