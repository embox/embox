/**
 * @file
 *
 * @date Aug 4, 2014
 * @author: Denis Deryugin
 */

#ifndef TASK_FORK_H_
#define TASK_FORK_H_

struct task;

struct stack_space {
	void *user_stack;
	size_t user_stack_sz;
};

struct heap_space {
	void *heap;
	size_t heap_sz;

	struct dlist_head store_space;
};

struct static_space {
	void *bss_store;
	void *data_store;
};

struct addr_space {
	struct addr_space *parent_addr_space;
	unsigned int child_count;

	struct pt_regs *pt_entry;

	struct thread *parent_thread;

	struct stack_space stack_space;
	struct heap_space heap_space;
	struct static_space static_space;
};

extern struct addr_space *fork_addr_space_get(const struct task *task);
extern void fork_set_addr_space(struct task *tk, struct addr_space *adrspc);
extern void fork_addr_space_delete(struct addr_space *adrspc);

#endif /* TASK_FORK_H_ */

