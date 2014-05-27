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
#include <asm/traps.h>

#include <hal/vfork.h> //TODO remove me

struct stack_space {
	struct thread *parent_thread;
	void *user_stack;
	size_t user_stack_sz;
};

struct addr_space {
	unsigned int use_count;

	struct pt_regs *pt_entry;

	struct stack_space stack_space;

	void *heap;
	void *statics;
};

static inline struct task *thread_get_task(struct thread *th) {
	return th->task;
}

static struct addr_space **fork_get_addr_space(struct task *ts);

static void fork_user_stack_store(struct stack_space *stspc) {
	size_t st_size;

	st_size = thread_stack_get_size(stspc->parent_thread);

	if (stspc->user_stack_sz != st_size) {
		if (stspc->user_stack) {
			sysfree(stspc->user_stack);
		}

		stspc->user_stack = sysmalloc(st_size);
		assert(stspc->user_stack); /* allocation successed */
		stspc->user_stack_sz = st_size;
	}

	memcpy(stspc->user_stack, thread_stack_get(stspc->parent_thread), st_size);
}

static void fork_user_stack_restore(struct stack_space *stspc, void *stack_safe_point) {
	void *stack = thread_stack_get(stspc->parent_thread);

	if (stack <= stack_safe_point && stack_safe_point < stack + stspc->user_stack_sz) {
		off_t off = stack_safe_point - stack;
		size_t sz = stspc->user_stack_sz - off;

		memcpy(stack + off, stspc->user_stack + off, sz);
	} else {
		memcpy(stack, stspc->user_stack, stspc->user_stack_sz);
	}
}

static void fork_user_stack_cleanup(struct stack_space *stspc) {

	if (stspc->user_stack) {
		sysfree(stspc->user_stack);
	}

	stspc->user_stack_sz = 0;
}

static struct addr_space *fork_addr_space_create(struct thread *current_thread) {
	struct addr_space *adrspc;

	adrspc = sysmalloc(sizeof(*adrspc));

	memset(adrspc, 0, sizeof(*adrspc));

	adrspc->stack_space.parent_thread = current_thread;
	adrspc->use_count = 1;

	return adrspc;
}

static void fork_addr_space_store(struct addr_space *adrspc) {
	fork_user_stack_store(&adrspc->stack_space);
}

static void fork_addr_space_restore(struct addr_space *adrspc, void *stack_safe_point) {
	fork_user_stack_restore(&adrspc->stack_space, stack_safe_point);
}

static void fork_addr_space_delete(struct addr_space *adrspc) {
	fork_user_stack_cleanup(&adrspc->stack_space);
	sysfree(adrspc);
}

static void *fork_child_trampoline(void *arg) {
	struct addr_space **adrspc_p = fork_get_addr_space(task_self());
	struct addr_space *adrspc = *adrspc_p;
	struct pt_regs ptregs;

	memcpy(&ptregs, adrspc->pt_entry, sizeof(ptregs));
	sysfree(adrspc->pt_entry);
	adrspc->pt_entry = NULL;

	ptregs_retcode(&ptregs, 0);
	vfork_leave(&ptregs);
	panic("%s returning", __func__);
}

void __attribute__((noreturn)) fork_body(struct pt_regs *ptregs) {
	struct addr_space *adrspc, *child_adrspc;
	pid_t child_pid;

	adrspc = fork_addr_space_create(thread_self());
	*fork_get_addr_space(task_self()) = adrspc;

	child_adrspc = fork_addr_space_create(thread_self());
	fork_addr_space_store(child_adrspc);

	child_adrspc->pt_entry = sysmalloc(sizeof(*adrspc->pt_entry));
	assert(child_adrspc->pt_entry);
	memcpy(child_adrspc->pt_entry, ptregs, sizeof(*child_adrspc->pt_entry));

	sched_lock();
	{
		child_pid = new_task("", fork_child_trampoline, NULL);
		*fork_get_addr_space(task_table_get(child_pid)) = child_adrspc;
	}
	sched_unlock();

	ptregs_retcode(ptregs, child_pid);
	vfork_leave(ptregs);

	panic("%s returning", __func__);
}

static void task_res_addr_space_ptr_init(const struct task *task, void *space) {
	struct addr_space **adrspc_p = space;

	*adrspc_p = NULL;
};

static void task_res_addr_space_ptr_deinit(const struct task *task) {
	struct addr_space **adrspc_p = fork_get_addr_space((struct task *) task); //FIXME const modifier dropped

	if (*adrspc_p) {
		fork_addr_space_delete(*adrspc_p);
	}
};

TASK_RESOURCE_DECLARE(static,
		task_resource_fork_addr_space,
		struct addr_space *,
	.init = task_res_addr_space_ptr_init,
	.deinit = task_res_addr_space_ptr_deinit,
);

static struct addr_space **fork_get_addr_space(struct task *ts) {
	return task_resource(ts, &task_resource_fork_addr_space);
}

void addr_space_store(void) {
	struct addr_space **adrspc_p = fork_get_addr_space(task_self());
	if (*adrspc_p) {
		fork_addr_space_store(*adrspc_p);
	}
}

void addr_space_restore(void *safe_point) {
	struct addr_space **adrspc_p = fork_get_addr_space(task_self());

	if (*adrspc_p) {
		fork_addr_space_restore(*adrspc_p, safe_point);
	}
}

#if 0
static int task_addr_space_change(struct thread *prev, struct thread *next) {
	struct addr_space *next_adrspc, *prev_adrspc;

	next_adrspc = *fork_get_addr_space_th(next);
	prev_adrspc = *fork_get_addr_space_th(prev);

	if (prev_adrspc) {
		fork_addr_space_store(prev_adrspc);
	}

	if (next_adrspc) {
		fork_addr_space_restore(next_adrspc);
	}

	return 0;
}

TASK_RESOURCE_NOTIFY(task_addr_space_change);
#endif
