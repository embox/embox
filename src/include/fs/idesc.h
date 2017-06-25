/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#ifndef FS_IDESC_H_
#define FS_IDESC_H_

struct idesc_ops;
struct idesc_xattrops;

#include <sys/types.h>

#include <kernel/sched/waitq.h>

struct idesc {
	mode_t idesc_amode;
	struct waitq idesc_waitq;
	const struct idesc_ops *idesc_ops;
	const struct idesc_xattrops *idesc_xattrops;
	unsigned int idesc_flags;
	int idesc_count;
};

struct iovec;
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
};

struct idesc_xattrops {
	int (*getxattr)(struct idesc *idesc, const char *name, void *value,
			size_t size);
	int (*setxattr)(struct idesc *idesc, const char *name,
			const void *value, size_t size, int flags);
	int (*listxattr)(struct idesc *idesc, char *list, size_t size);
	int (*removexattr)(struct idesc *idesc, const char *name);
};

#include <sys/cdefs.h>
__BEGIN_DECLS

extern int idesc_init(struct idesc *idesc, const struct idesc_ops *ops,
		mode_t amode);

extern int idesc_check_mode(struct idesc *idesc, mode_t amode);

extern int idesc_getxattr(struct idesc *idesc, const char *name, void *value,
		size_t size);
extern int idesc_setxattr(struct idesc *idesc, const char *name, const void *value,
		size_t size, int flags);
extern int idesc_listxattr(struct idesc *idesc, char *list, size_t size);

extern int idesc_removexattr(struct idesc *idesc, const char *name);

extern int idesc_close(struct idesc *idesc, int fd);

__END_DECLS

#endif /* FS_IDESC_H_ */
