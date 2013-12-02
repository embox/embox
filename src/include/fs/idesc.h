/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#ifndef FS_IDESC_H_
#define FS_IDESC_H_

#include <fs/flags.h>


struct idesc_ops;
typedef int idesc_access_mode_t;

#include <kernel/sched/waitq.h>

struct idesc {
	idesc_access_mode_t idesc_amode;
	struct waitq idesc_waitq;
	const struct idesc_ops *idesc_ops;
	unsigned int idesc_flags;
	int idesc_count;
};

/**
 * Specify operations index descriptor can implement itself
 */
struct idesc_ops {
	int (*read)(struct idesc *idesc, void *buf, size_t nbyte);
	int (*write)(struct idesc *idesc, const void *buf, size_t nbyte);
	int (*close)(struct idesc *idesc);
	int (*ioctl)(struct idesc *idesc, int request, void *);
	int (*fstat)(struct idesc *idesc, void *buff);
	int (*status)(struct idesc *idesc, int mask);
};


#include <sys/cdefs.h>
__BEGIN_DECLS

extern int idesc_init(struct idesc *idesc, const struct idesc_ops *ops,
		idesc_access_mode_t amode);

extern int idesc_check_mode(struct idesc *idesc, idesc_access_mode_t amode);

#if 0
extern int idesc_read(struct idesc *idesc, char *buff, int size);

extern int idesc_write(struct idesc *idesc,char *buff, int size);

extern int idesc_ctrl(struct idesc *idesc, int cmd, int req, void *buff,
		int size);
#endif

extern int idesc_close(struct idesc *idesc, int fd);

__END_DECLS

#endif /* FS_IDESC_H_ */
