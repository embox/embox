/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_SYSCALL_CALLER_H_
#define KERNEL_SYSCALL_CALLER_H_

#include <module/embox/arch/syscall_caller.h>

#define SYSCALL0(NR,type,name)                                               \
	  __SYSCALL0(NR,type,name)

#define SYSCALL1(NR,type,name,type1,arg1)                                    \
	  __SYSCALL1(NR,type,name,type1,arg1)

#define SYSCALL2(NR,type,name,type1,arg1,type2,arg2)                         \
	  __SYSCALL2(NR,type,name,type1,arg1,type2,arg2)

#define SYSCALL3(NR,type,name,type1,arg1,type2,arg2,type3,arg3)              \
      __SYSCALL3(NR,type,name,type1,arg1,type2,arg2,type3,arg3)

#define SYSCALL4(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)   \
      __SYSCALL4(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)

#define SYSCALL5(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,   \
          type5,arg5)                                                        \
      __SYSCALL5(NR,type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,   \
          type5,arg5)

#endif /* KERNEL_SYSCALL_CALLER_H_ */
