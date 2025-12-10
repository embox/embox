/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.06.2014
 */

#ifndef FORK_COPY_ADDR_SPACE_H_
#define FORK_COPY_ADDR_SPACE_H_

#include <stddef.h>

#include <hal/ptrace.h>
#include <hal/stack.h>
#include <kernel/task.h>
#include <kernel/task/resource/module_ptr.h>
#include <lib/libds/dlist.h>

struct stack_space {
	struct dlist_head list;
	struct thread *thread;
	void *stack;
	size_t stack_sz;
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

	struct pt_regs pt_entry;

	struct dlist_head stack_space_head;
	struct heap_space heap_space;
	struct static_space static_space;
};

extern void fork_addr_space_prepare_switch();
extern void fork_addr_space_finish_switch(void *safe_point);

#define __ADDR_SPACE_PREPARE_SWITCH() fork_addr_space_prepare_switch()
#define __ADDR_SPACE_FINISH_SWITCH()  fork_addr_space_finish_switch(stack_ptr())

#define __ADDR_SPACE_PREPARE_APP(mod) task_self_module_ptr_set(mod)
#define __ADDR_SPACE_FINISH_APP()     task_self_module_ptr_set(NULL)

/* Stack */
struct thread;
extern void fork_stack_store(struct addr_space *adrspc, struct thread *th);
extern void fork_stack_restore(struct addr_space *adrspc, void *stack_safe_point);
extern void fork_stack_cleanup(struct addr_space *adrspc);

/* Heap */
extern void fork_heap_store(struct heap_space *hpspc, struct task *tk);
extern void fork_heap_restore(struct heap_space *hpspc);
extern void fork_heap_cleanup(struct heap_space *hpspc);

/* Static */
extern void fork_static_store(struct static_space *sspc);
extern void fork_static_restore(struct static_space *sspc);
extern void fork_static_cleanup(struct static_space *sspc);

#endif /* FORK_COPY_ADDR_SPACE_H_ */
