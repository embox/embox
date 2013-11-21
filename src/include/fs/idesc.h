/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#ifndef FS_IDESC_H_
#define FS_IDESC_H_


typedef int discrete_permission_t;
struct mandatoty_permission {

};
typedef struct mandatoty_permission mandatoty_permission_t;

struct idesc_perm {
	discrete_permission_t dperm;
	mandatoty_permission_t mperm;
};

#include <kernel/task/io_sync.h>

struct idesc_event {
	struct io_sync io_sync;
};


struct task_idx_ops;
struct idesc {
	struct idesc_perm idesc_perm;
	struct idesc_event idesc_event;
	const struct task_idx_ops *idesc_ops;
	unsigned int idesc_flags;
	int idesc_count;
};


#include <sys/cdefs.h>
__BEGIN_DECLS

extern int idesc_perm_init(struct idesc_perm *iperm, discrete_permission_t dperm);

extern int idesc_init(struct idesc *idesc, const struct task_idx_ops *ops, struct idesc_perm *attr);

extern int idesc_read(struct idesc *idesc, char *buff, int size);

extern int idesc_write(struct idesc *idesc,char *buff, int size);

extern int idesc_ctrl(struct idesc *idesc, int cmd, int req, void *buff,
		int size);

extern int idesc_close(struct idesc *idesc, int fd);

__END_DECLS

#endif /* FS_IDESC_H_ */
