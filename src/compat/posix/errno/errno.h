
#ifndef POSIX_ERRNO_ERRNO_H_
#define POSIX_ERRNO_ERRNO_H_

#include <kernel/task/resource/errno.h>

#define errno *task_self_resource_errno()

#endif /* POSIX_ERRNO_ERRNO_H_ */
