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

struct idesc_permissions {
	discrete_permission_t dperm;
	mandatoty_permission_t mperm;
};


struct idesc {
	struct idesc_permission perm;
	struct idesc_event;
};


#include <sys/cdefs.h>
__BEGIN_DECLS

extern int idesc_init(struct idesc *idesc, struct idesc_permissions *attr);

extern int idesc_read(struct idesc *idesc, char *buff, int size);

extern int idesc_write(struct idesc *idesc,char *buff, int size);

extern int idesc_ctrl(struct idesc *idesc, int cmd, int req, void *buff,
		int size);

__END_DECLS

#endif /* FS_IDESC_H_ */
