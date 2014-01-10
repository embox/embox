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
}

struct cpu_stack setjmp_stack_get(void *stack_base, jmp_buf env) {
	struct cpu_stack stack;

	stack.ebp = (ptrdiff_t)stack_base - env[SETJMP_EBP_INDEX];
	stack.esp = (ptrdiff_t)stack_base - env[SETJMP_ESP_INDEX];

	return stack;
}
/*
void cpu_stack_move(ptrdiff_t offset) {
	int esp_reg;
	int ebp_reg;

	asm volatile("movl %0, %%eax "
			""
			":");

}
*/
extern void cpu_stack_move(ptrdiff_t offset);
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

	cpu_stack_move(stack_offset);

//	setjmp_stack_get()
	stack = setjmp_stack_get(thr->stack.stack, data->kfork_jmp_buf);
	stack_offset = stack.ebp + 0x200;
	memcpy(cur_thr->stack.stack, thr->stack.stack, stack_offset);


	longjmp(data->kfork_jmp_buf, 1);

	return 0;
}

pid_t kfork(void) {
	struct task *parent_task;
	struct thread *par_thread;
	pid_t child_pid;

	parent_task = task_self();
	par_thread = thread_self();

	assert(parent_task == par_thread->task);

	kfork_data.par_thr = par_thread;

	switch(setjmp(kfork_data.kfork_jmp_buf)) {
	case 0:
		child_pid = new_task(parent_task->task_name, kfork_trampoline, &kfork_data);
		return child_pid;
	default:
		return 0;
	}
}
