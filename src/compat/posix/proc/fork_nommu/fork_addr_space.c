/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    30.07.2014
 */

#include <sys/types.h>
#include <assert.h>

#include "fork_copy_addr_space.h"
#include <kernel/task/resource.h>
#include <kernel/task/resource/task_fork.h>
#include <mem/sysmalloc.h>

static int fork_addr_space_is_shared(struct addr_space *adrspc) {
	return adrspc->parent_addr_space || adrspc->child_count;
}

void fork_addr_space_prepare_switch() {
	struct addr_space *adrspc;

	adrspc = fork_addr_space_get(task_self());

	if (!adrspc)
		return;

	if (!fork_addr_space_is_shared(adrspc))
		return;

	fork_addr_space_store(adrspc);
}


void fork_addr_space_finish_switch(void *safe_point) {
	struct addr_space *adrspc;

	assert(safe_point);

	adrspc = fork_addr_space_get(task_self());
	if (!adrspc) {
		return;
	}

	fork_addr_space_restore(adrspc, safe_point);

	if (!fork_addr_space_is_shared(adrspc)) {
		fork_addr_space_delete(task_self());
	}
}

struct addr_space *fork_addr_space_create(struct addr_space *parent) {
	struct addr_space *adrspc;

	adrspc = sysmalloc(sizeof(*adrspc));
	memset(adrspc, 0, sizeof(*adrspc));

	dlist_init(&adrspc->stack_space_head);

	if (parent) {
		adrspc->parent_addr_space = parent;
		parent->child_count++;
	}

	return adrspc;
}

void fork_addr_space_store(struct addr_space *adrspc) {
	fork_stack_store(adrspc, thread_self());
	fork_heap_store(&adrspc->heap_space, task_self());
	fork_static_store(&adrspc->static_space);
}

void fork_addr_space_restore(struct addr_space *adrspc, void *stack_safe_point) {
	assert(adrspc);
	assert(stack_safe_point);
	fork_stack_restore(adrspc, stack_safe_point);
	fork_heap_restore(&adrspc->heap_space);
	fork_static_restore(&adrspc->static_space);
}

static void fork_addr_space_child_del(struct addr_space *child) {
	struct addr_space *parent;

	assertf(child->child_count == 0, "%s: deleting address space with children is NIY", __func__);

	parent = child->parent_addr_space;
	if (!parent) {
		return;
	}

	/* removing child from parent */
	parent->child_count--;

	/* reassigning child's childs to parent */
	parent->child_count += child->child_count;
}

static void fork_addr_space_init(const struct task *task, void *space) {
	struct addr_space **adrspc_p = space;

	*adrspc_p = NULL;
}

static void fork_addr_space_deinit(const struct task *task) {
	assert(task == task_self());
	fork_addr_space_delete((struct task *)task);
}

TASK_RESOURCE_DECLARE(static,
		fork_addr_space,
		struct addr_space *,
	.init = fork_addr_space_init,
	.deinit = fork_addr_space_deinit,
);

struct addr_space *fork_addr_space_get(const struct task *task) {
	return *((struct addr_space **) task_resource(task, &fork_addr_space));
}

void fork_addr_space_set(struct task *tk, struct addr_space *adrspc) {
	struct addr_space **adrspc_p;
	adrspc_p = task_resource(tk, &fork_addr_space);
	*adrspc_p = adrspc;
}

void fork_addr_space_delete(struct task *task) {
	struct addr_space *adrspc;
	adrspc = fork_addr_space_get(task);

	if (!adrspc)
		return;

	fork_stack_cleanup(adrspc);
	fork_heap_cleanup(&adrspc->heap_space);
	fork_static_cleanup(&adrspc->static_space);

	fork_addr_space_child_del(adrspc);

	sysfree(adrspc);

	fork_addr_space_set(task, NULL);
}
