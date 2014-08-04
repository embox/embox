/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.05.2014
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

extern size_t mspace_deep_copy_size(struct dlist_head *mspace);
extern void mspace_deep_store(struct dlist_head *mspace, struct dlist_head *store_space, void *buf);
extern void mspace_deep_restore(struct dlist_head *mspace, struct dlist_head *store_space, void *buf);

static inline struct dlist_head *task_mspace(struct task *tk) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task_self());
	return &task_heap->mm;
}

static void fork_user_stack_store(struct stack_space *stspc, struct thread *thread) {
	size_t st_size;

	st_size = thread_stack_get_size(thread);

	if (stspc->user_stack_sz != st_size) {
		if (stspc->user_stack) {
			sysfree(stspc->user_stack);
		}

		stspc->user_stack = sysmalloc(st_size);
		assert(stspc->user_stack); /* allocation successed */
		stspc->user_stack_sz = st_size;
	}

	memcpy(stspc->user_stack, thread_stack_get(thread), st_size);
}

static void fork_user_stack_restore(struct stack_space *stspc, struct thread *th, void *stack_safe_point) {
	void *stack = thread_stack_get(th);

	if (stack <= stack_safe_point && stack_safe_point < stack + stspc->user_stack_sz) {
		off_t off = stack_safe_point - stack;
		size_t sz = stspc->user_stack_sz - off;

		memcpy(stack + off, stspc->user_stack + off, sz);
	} else {
		memcpy(stack, stspc->user_stack, stspc->user_stack_sz);
	}
}

static void fork_heap_store(struct heap_space *hpspc, struct task *tk) {
	size_t size;

	size = mspace_deep_copy_size(task_mspace(tk));

	if (hpspc->heap_sz != size) {
		if (hpspc->heap) {
			page_free(__phymem_allocator, hpspc->heap, hpspc->heap_sz / PAGE_SIZE());
		}

		hpspc->heap = page_alloc(__phymem_allocator, size / PAGE_SIZE());
		assert(hpspc->heap);
	}
	mspace_deep_store(task_mspace(tk), &hpspc->store_space, hpspc->heap);
}

static void fork_heap_restore(struct heap_space *hpspc, struct task *tk) {
	mspace_deep_restore(task_mspace(tk), &hpspc->store_space, hpspc->heap);
}

static inline const struct mod_app *task_app_get(struct task *tk) {
	const struct mod *mod = task_module_ptr_get(tk);
	return mod ? mod->app : NULL;
}

static void fork_static_store(struct static_space *sspc, struct task *tk) {
	const struct mod_app *app;

	app = task_app_get(tk);
	if (!app) {
		return;
	}

	if (!sspc->bss_store) {
		sspc->bss_store = sysmalloc(app->bss_sz);
		assert(sspc->bss_store);
	}
	memcpy(sspc->bss_store, app->bss, app->bss_sz);

	if (!sspc->data_store) {
		sspc->data_store = sysmalloc(app->data_sz);
		assert(sspc->data_store);
	}
	memcpy(sspc->data_store, app->data, app->data_sz);
}

static void fork_static_restore(struct static_space *sspc, struct task *tk) {
	const struct mod_app *app;

	app = task_app_get(tk);
	if (!app) {
		return;
	}

	assert(sspc->bss_store);
	assert(sspc->data_store);

	memcpy(app->bss, sspc->bss_store, app->bss_sz);
	memcpy(app->data, sspc->data_store, app->data_sz);
}

static void fork_addr_space_child_add(struct addr_space *parent, struct addr_space *child) {
	child->parent_addr_space = parent;

	if (parent) {
		parent->child_count++;
	}
}

static struct addr_space *fork_addr_space_create(struct thread *current_thread, struct addr_space *parent) {
	struct addr_space *adrspc;

	adrspc = sysmalloc(sizeof(*adrspc));

	memset(adrspc, 0, sizeof(*adrspc));

	adrspc->parent_thread = current_thread;

	adrspc->child_count = 0;

	fork_addr_space_child_add(parent, adrspc);

	return adrspc;
}

static void fork_addr_space_store(struct addr_space *adrspc) {
	struct task *tk = task_self();

	fork_user_stack_store(&adrspc->stack_space, adrspc->parent_thread);
	fork_heap_store(&adrspc->heap_space, tk);
	fork_static_store(&adrspc->static_space, tk);
}

static void fork_addr_space_restore(struct addr_space *adrspc, void *stack_safe_point) {
	struct task *tk = task_self();

	fork_user_stack_restore(&adrspc->stack_space, adrspc->parent_thread, stack_safe_point);
	fork_heap_restore(&adrspc->heap_space, tk);
	fork_static_restore(&adrspc->static_space, tk);
}

static void *fork_child_trampoline(void *arg) {
	struct addr_space *adrspc;
	struct pt_regs ptregs;

	adrspc = fork_addr_space_get(task_self());
	memcpy(&ptregs, adrspc->pt_entry, sizeof(ptregs));
	sysfree(adrspc->pt_entry);
	adrspc->pt_entry = NULL;

	ptregs_retcode_jmp(&ptregs, 0);
	panic("%s returning", __func__);
}

void __attribute__((noreturn)) fork_body(struct pt_regs *ptregs) {
	struct addr_space *adrspc, *child_adrspc;
	pid_t child_pid;

	adrspc = fork_addr_space_get(task_self());
	if (!adrspc) {
		adrspc = fork_addr_space_create(thread_self(), NULL);
		fork_set_addr_space(task_self(), adrspc);
	}

	child_adrspc = fork_addr_space_create(thread_self(), adrspc);
	fork_addr_space_store(child_adrspc);

	child_adrspc->pt_entry = sysmalloc(sizeof(*adrspc->pt_entry));
	assert(child_adrspc->pt_entry);
	memcpy(child_adrspc->pt_entry, ptregs, sizeof(*child_adrspc->pt_entry));

	sched_lock();
	{
		child_pid = new_task("", fork_child_trampoline, NULL);
		fork_set_addr_space(task_table_get(child_pid), child_adrspc);
	}
	sched_unlock();

	ptregs_retcode_jmp(ptregs, child_pid);

	panic("%s returning", __func__);
}

void fork_addr_space_prepare_switch(void) {
	struct addr_space *adrspc = fork_addr_space_get(task_self());
	if (adrspc) {
		fork_addr_space_store(adrspc);
	}
}

static int fork_addr_space_is_shared(struct addr_space *adrspc) {
	return adrspc->parent_addr_space || adrspc->child_count;
}

void fork_addr_space_finish_switch(void *safe_point) {
	struct addr_space *adrspc = fork_addr_space_get(task_self());

	if (adrspc) {

		fork_addr_space_restore(adrspc, safe_point);

		if (!fork_addr_space_is_shared(adrspc)) {
			fork_addr_space_delete(adrspc);
			fork_set_addr_space(task_self(), NULL);
		}
	}
}
