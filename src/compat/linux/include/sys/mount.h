/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.03.24
 */

#ifndef COMPAT_LINUX_SYS_MOUNT_H_
#define COMPAT_LINUX_SYS_MOUNT_H_

#include <sys/cdefs.h>

#define MS_RDONLY      (1 << 0)  /* Mount read-only */
#define MS_NOSUID      (1 << 1)  /* Ignore suid and sgid bits */
#define MS_NODEV       (1 << 2)  /* Disallow access to device special files */
#define MS_NOEXEC      (1 << 3)  /* Disallow program execution */
#define MS_SYNCHRONOUS (1 << 4)  /* Writes are synced at once */
#define MS_REMOUNT     (1 << 5)  /* Remount an existing mount */
#define MS_MANDLOCK    (1 << 6)  /* Allow mandatory locks on an FS */
#define MS_DIRSYNC     (1 << 7)  /* Directory modifications are synchronous */
#define MS_NOSYMFOLLOW (1 << 8)  /* Do not follow symlinks */
#define MS_NOATIME     (1 << 9)  /* Do not update access times. */
#define MS_NODIRATIME  (1 << 10) /* Do not update directory access times */
#define MS_BIND        (1 << 11) /* Perform a bind mount */
#define MS_MOVE        (1 << 12) /* Move a subtree */

__BEGIN_DECLS

extern int mount(const char *source, const char *target,
    const char *filesystemtype, unsigned long mountflags, const void *data);

extern int umount(const char *target);

/* 
extern int umount2(const char *target, int flags);
 */

__END_DECLS

#endif /* COMPAT_LINUX_SYS_MOUNT_H_ */
