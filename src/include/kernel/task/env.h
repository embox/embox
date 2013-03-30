/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 30.03.13
 */

#ifndef KERNEL_TASK_ENV_H_
#define KERNEL_TASK_ENV_H_

#include <framework/mod/options.h>
#include <stddef.h>
#include <sys/cdefs.h>

#include <module/embox/lib/LibCStdlib.h>
#define MODOPS_ENV_PER_TASK \
	OPTION_MODULE_GET(embox__lib__LibCStdlib, NUMBER, env_per_task)

__BEGIN_DECLS

struct env_struct {
	char **env;
	char *vals[MODOPS_ENV_PER_TASK + 1];
	size_t next;
};

extern struct env_struct * task_self_env_ptr(void);

extern char *** task_self_environ_ptr(void);

__END_DECLS

#endif /* KERNEL_TASK_ENV_H_ */
