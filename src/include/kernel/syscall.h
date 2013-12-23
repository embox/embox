/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_SYSCALL_H_
#define KERNEL_SYSCALL_H_

extern int sys_exit(int errcode);
extern int sys_write(int fd, const char *buf, int count);

#include <module/embox/kernel/syscall_context.h>

extern void syscall_enter();
extern void syscall_exit();

#endif /* KERNEL_SYSCALL_H_ */
