/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#ifndef KERNEL_TASK_IDESC_IDESC_H_
#define KERNEL_TASK_IDESC_IDESC_H_

#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/sched/waitq.h>

struct iovec;
struct idesc_ops;
struct idesc_xattrops;

struct idesc {
	struct waitq idesc_waitq;
	const struct idesc_ops *idesc_ops;
	const struct idesc_xattrops *idesc_xattrops;
	unsigned int idesc_flags;
	int idesc_count;
};

/**
 * Specify operations index descriptor can implement itself
 */
struct idesc_ops {
	ssize_t (*id_readv)(struct idesc *idesc, const struct iovec *iov, int cnt);
	ssize_t (*id_writev)(struct idesc *idesc, const struct iovec *iov, int cnt);
	void (*close)(struct idesc *idesc);
	int (*ioctl)(struct idesc *idesc, int request, void *data);
	int (*fstat)(struct idesc *idesc, void *buff);
	int (*status)(struct idesc *idesc, int mask);
	void *(*idesc_mmap)(struct idesc *idesc, void *addr, size_t len, int prot,
	    int flags, int fd, off_t off);
};

struct idesc_xattrops {
	int (*getxattr)(struct idesc *idesc, const char *name, void *value,
	    size_t size);
	int (*setxattr)(struct idesc *idesc, const char *name, const void *value,
	    size_t size, int flags);
	int (*listxattr)(struct idesc *idesc, char *list, size_t size);
	int (*removexattr)(struct idesc *idesc, const char *name);
};

__BEGIN_DECLS

extern int idesc_init(struct idesc *idesc, const struct idesc_ops *ops,
    mode_t amode);

extern int idesc_check_mode(struct idesc *idesc, mode_t amode);

extern int idesc_getxattr(struct idesc *idesc, const char *name, void *value,
    size_t size);

extern int idesc_setxattr(struct idesc *idesc, const char *name,
    const void *value, size_t size, int flags);

extern int idesc_listxattr(struct idesc *idesc, char *list, size_t size);

extern int idesc_removexattr(struct idesc *idesc, const char *name);

extern int idesc_close(struct idesc *idesc, int fd);

__END_DECLS

#endif /* KERNEL_TASK_IDESC_IDESC_H_ */
