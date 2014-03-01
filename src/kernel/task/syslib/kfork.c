/**
 * @file
 *
 * @date Jan 9, 2014
 * @author: Anton Bondarev
 */
#include <assert.h>
#include <sys/types.h>
#include <setjmp.h>


#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_stack.h>
#include <kernel/time/ktime.h>


struct kfork_data {
	struct thread *par_thr;
	jmp_buf kfork_jmp_buf;
};

static struct kfork_data kfork_data;
static int tmp_stack_buff[0x1000];

struct cpu_stack {
	int ebp;
	int esp;
};

void setjmp_stack_move(ptrdiff_t offset, jmp_buf env) {
	env[SETJMP_EBP_INDEX] += offset;
	env[SETJMP_ESP_INDEX] += offset;

	*(uint32_t *)env[SETJMP_EBP_INDEX] += offset;
}

struct cpu_stack setjmp_stack_get(void *stack_base, jmp_buf env) {
	struct cpu_stack stack;

	stack.ebp = (ptrdiff_t)stack_base - env[SETJMP_EBP_INDEX];
	stack.esp = (ptrdiff_t)stack_base - env[SETJMP_ESP_INDEX];

	return stack;
}

extern void cpu_stack_move(ptrdiff_t desc_stack, ptrdiff_t src_stack);
static void *kfork_trampoline(void *arg) {
	struct thread *thr;
	struct thread *cur_thr;
	struct kfork_data *data;
	ptrdiff_t stack_offset;
	struct cpu_stack stack;

	assert(arg);
	data = arg;

	thr = data->par_thr;
	cur_thr = thread_self();

	memcpy(tmp_stack_buff,
			(void *)((intptr_t)cur_thr->stack.stack +
			cur_thr->stack.stack_sz - sizeof(tmp_stack_buff)),
			sizeof(tmp_stack_buff));

	stack_offset = (ptrdiff_t)cur_thr->stack.stack + cur_thr->stack.stack_sz -
			((ptrdiff_t)tmp_stack_buff + sizeof(tmp_stack_buff));

	cpu_stack_move((ptrdiff_t)tmp_stack_buff + sizeof(tmp_stack_buff),
			(ptrdiff_t)cur_thr->stack.stack + cur_thr->stack.stack_sz);


	stack = setjmp_stack_get(
			thr->stack.stack + thr->stack.stack_sz,
			data->kfork_jmp_buf);


	stack_offset = stack.ebp + 0x200;
	memcpy(cur_thr->stack.stack + cur_thr->stack.stack_sz - stack_offset,
			thr->stack.stack + thr->stack.stack_sz- stack_offset,
			stack_offset);

	stack_offset = (ptrdiff_t)cur_thr->stack.stack + cur_thr->stack.stack_sz -
				((ptrdiff_t)thr->stack.stack + thr->stack.stack_sz);

	setjmp_stack_move(stack_offset, data->kfork_jmp_buf);


	longjmp(data->kfork_jmp_buf, 1);

	return 0;
}

pid_t kfork(jmp_buf env) {
	struct task *parent_task;
	struct thread *par_thread;
	pid_t child_pid;

	parent_task = task_self();
	par_thread = thread_self();

	assert(parent_task == par_thread->task);

	kfork_data.par_thr = par_thread;
	memcpy(kfork_data.kfork_jmp_buf, env, sizeof(jmp_buf));

	child_pid = new_task(task_get_name(parent_task), kfork_trampoline, &kfork_data);
	ksleep(0);
	return child_pid;
}
