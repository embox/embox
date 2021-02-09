/**
 * @file
 *
 * @date Feb 3, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LIBC_STDIO_STDOUT_TASK_RESOURCE_H_
#define SRC_COMPAT_LIBC_STDIO_STDOUT_TASK_RESOURCE_H_

#include <module/embox/kernel/task/resource/stdout.h>

#define stdout task_self_resource_sdtout()

#endif /* SRC_COMPAT_LIBC_STDIO_STDOUT_TASK_RESOURCE_H_ */
