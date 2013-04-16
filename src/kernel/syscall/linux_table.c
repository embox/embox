/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Anton Bulychev
 */


#include <stdint.h>
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>

#define SYSCALL_NRS_TOTAL 200

extern int sys_fork(void);
extern long sys_exit(int errcode);
extern size_t sys_write(int fd, const void *buf, size_t nbyte);
extern void *sys_mmap2(void *start, size_t length, int prot, int flags, int fd, uint32_t pgoffset);
extern int sys_open(const char *path, int flags, mode_t mode);
extern int sys_close(int fd);
extern int sys_newfstat(int fd, void *buf);
extern void *sys_brk(void *new_brk);

void *const SYSCALL_TABLE[SYSCALL_NRS_TOTAL] = {
	NULL, sys_exit, sys_fork, NULL, sys_write, sys_open, sys_close, NULL, NULL, NULL,    // 0 - 9
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 10 - 19
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 20 - 29
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 30 - 39
	NULL, NULL, NULL, NULL, NULL, sys_brk, NULL, NULL, NULL, NULL,    // 40 - 49
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 50
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 60
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 70
 	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 80
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 90
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, sys_newfstat, NULL,    // 100
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 110
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 120
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 130
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 140
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 150
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 160
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 170
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 180
	NULL, NULL, sys_mmap2, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    // 190
};
