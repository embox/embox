/**
 * @file
 *
 * @date Nov 29, 2013
 * @author: Anton Bondarev
 */

#ifndef INDEX_DESCRIPTOR_H_
#define INDEX_DESCRIPTOR_H_

#include <sys/cdefs.h>
__BEGIN_DECLS

struct idesc;

extern int index_descritor_cloexec_get(int fd);

extern int index_descriptor_cloexec_set(int fd, int cloexec);

extern struct idesc *index_descriptor_get(int idx);

extern int index_descriptor_add(struct idesc *idesc);

extern int index_descriptor_flags_get(int fd);

extern int index_descriptor_flags_set(int fd, int flags);

extern int index_descriptor_ioctl(int fd, int req, void *data);

extern int index_descriptor_fcntl(int fd, int cmd, void *data);

extern int index_descriptor_status(int fd, int pollmask);

extern int index_descriptor_dupfd(int fd, int newfd);

struct stat;
extern int index_descriptor_fstat(int fd, struct stat *buff);

__END_DECLS


#endif /* INDEX_DESCRIPTOR_H_ */
