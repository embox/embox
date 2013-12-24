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

#endif /* KERNEL_SYSCALL_H_ */
