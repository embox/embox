/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_WAIT_H_
#define SYS_WAIT_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#include <kernel/task/defs.h>

#define WCONTINUED      1
#define WNOHANG         2
#define WUNTRACED       4

#define WIFEXITED(stat_val)     ((~stat_val) & TASKST_EXITED_MASK)
#define WEXITSTATUS(stat_val)   ((stat_val) & TASKST_EXITST_MASK)
#define WIFSIGNALED(stat_val)   ((stat_val) & TASKST_SIGNALD_MASK)
#define WTERMSIG(stat_val)      ((stat_val) & TASKST_TERMSIG_MASK)
#define WIFSTOPPED(stat_val)    ((stat_val) & TASKST_STOPPED_MASK)
#define WSTOPSIG(stat_val)      ((stat_val) & TASKST_STOPSIG_MASK)
#define WIFCONTINUED(stat_val)  ((stat_val) & TASKST_CONT_MASK)

__BEGIN_DECLS
extern pid_t waitpid(pid_t pid, int *status, int options);
extern pid_t wait(int *status);
__END_DECLS

#endif /* SYS_WAIT_H_ */
