/**
 * @file
 *
 * @date Aug 4, 2014
 * @author: Denis Deryugin
 */

#ifndef TASK_FORK_H_
#define TASK_FORK_H_

#include <sys/types.h>
#include <lib/libds/dlist.h>

struct task;
extern struct addr_space *fork_addr_space_get(const struct task *task);
extern void fork_addr_space_set(struct task *tk, struct addr_space *adrspc);
extern void fork_addr_space_delete(struct task *task);
extern struct addr_space *fork_addr_space_create(struct addr_space *parent);
extern void fork_addr_space_store(struct addr_space *adrspc);
extern void fork_addr_space_restore(struct addr_space *adrspc, void *stack_safe_point);

#endif /* TASK_FORK_H_ */

