/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    30.07.2014
 */

#include <mem/sysmalloc.h>
#include <sys/types.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/task/resource.h>
#include <kernel/panic.h>
#include <hal/ptrace.h>

#include <mem/page.h>
#include <mem/phymem.h>

#include <kernel/task/resource/task_heap.h>
#include <kernel/task/resource/task_fork.h>

#include <kernel/task/resource/module_ptr.h>
#include <framework/mod/types.h>

static void fork_addr_space_child_del(struct addr_space *child) {
	struct addr_space *parent = child->parent_addr_space;

	assert(child->child_count == 0, "%s: deleting address space with childs is NIY", __func__);

	if (!parent) {
		return;
	}

	/* removing child from parent */
	parent->child_count--;

	/* reassigning child's childs to parent */
	parent->child_count += child->child_count;
}

static void fork_static_cleanup(struct static_space *sspc) {

	if (sspc->bss_store) {
		sysfree(sspc->bss_store);
	}

	if (sspc->data_store) {
		sysfree(sspc->data_store);
	}
}

static void fork_heap_cleanup(struct heap_space *hpspc) {
	if (hpspc->heap) {
		page_free(__phymem_allocator, hpspc->heap, hpspc->heap_sz / PAGE_SIZE());
	}
}

static void fork_user_stack_cleanup(struct stack_space *stspc) {

	if (stspc->user_stack) {
		sysfree(stspc->user_stack);
	}

	stspc->user_stack_sz = 0;
}

static void fork_addr_space_init(const struct task *task, void *space) {
	struct addr_space **adrspc_p = space;

	*adrspc_p = NULL;
};

static void fork_addr_space_deinit(const struct task *task) {
	struct addr_space *adrspc;

	assert(task == task_self());

	adrspc = fork_addr_space_get(task_self());

	if (adrspc) {
		fork_addr_space_delete(adrspc);
		fork_set_addr_space(task_self(), NULL);
	}
};

TASK_RESOURCE_DECLARE(static,
		fork_addr_space,
		struct addr_space *,
	.init = fork_addr_space_init,
	.deinit = fork_addr_space_deinit,
);

struct addr_space *fork_addr_space_get(const struct task *task) {
	return *((struct addr_space **) task_resource(task, &fork_addr_space));
}

void fork_set_addr_space(struct task *tk, struct addr_space *adrspc) {
	struct addr_space **adrspc_p;
	adrspc_p = task_resource(tk, &fork_addr_space);
	*adrspc_p = adrspc;
}

void fork_addr_space_delete(struct addr_space *adrspc) {

	fork_user_stack_cleanup(&adrspc->stack_space);
	fork_heap_cleanup(&adrspc->heap_space);
	fork_static_cleanup(&adrspc->static_space);

	fork_addr_space_child_del(adrspc);
	sysfree(adrspc);
}
