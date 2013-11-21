/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#ifndef FS_IDESC_H_
#define FS_IDESC_H_

#include <fs/flags.h>

struct task_idx_ops;
typedef int idesc_access_mode_t;

#include <kernel/task/io_sync.h>

struct idesc_event {
	struct io_sync io_sync;
};


struct idesc {
	idesc_access_mode_t idesc_amode;
	struct idesc_event idesc_event;
	const struct task_idx_ops *idesc_ops;
	unsigned int idesc_flags;
	int idesc_count;
};


#include <sys/cdefs.h>
__BEGIN_DECLS

extern int idesc_init(struct idesc *idesc, const struct task_idx_ops *ops,
		idesc_access_mode_t amode);

extern int idesc_read(struct idesc *idesc, char *buff, int size);

extern int idesc_write(struct idesc *idesc,char *buff, int size);

extern int idesc_ctrl(struct idesc *idesc, int cmd, int req, void *buff,
		int size);

extern int idesc_close(struct idesc *idesc, int fd);

__END_DECLS

#endif /* FS_IDESC_H_ */
