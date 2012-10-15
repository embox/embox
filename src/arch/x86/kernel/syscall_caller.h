/**
 * @file
 *
 * @date 15.10.2012
 * @author Anton Bulychev
 */

#ifndef X86_SYSCALL_CALLER_H_
#define X86_SYSCALL_CALLER_H_

#ifndef __ASSEMBLER__

extern int syscall_exit(int errcode);
extern int syscall_write(int fd, const char *bug, int count);

#endif /* __ASSEMBLER__ */

#endif /* X86_SYSCALL_CALLER_H_ */
