/**
 * @file
 * @brief
 *
 * @date 10.10.13
 * @author Ilia Vaprol
 */

#include <execinfo.h>

#include <debug/symbol.h>
#include <debug/whereami.h>
#include <kernel/printk.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched.h>
#include <hal/context.h>
#include <util/array.h>
#include <util/math.h>

#include <framework/mod/options.h>


extern int backtrace_context(void **buff, int size, struct context *ctx); // XXX

#define MODOPS_ROW_LIMIT OPTION_GET(NUMBER, row_limit)
#define MODOPS_MAX_DEPTH OPTION_GET(NUMBER, max_depth)

static void *bt_buff[MODOPS_MAX_DEPTH];

static void dump_thread_stack(struct thread *t) {
	struct context *ctx;
	int i, size, limit;
	const struct symbol *s;
	ptrdiff_t offset;
	int is_current = (t == thread_self());

	ctx  = is_current ? NULL : &t->context;
	size = backtrace_context(&bt_buff[0], ARRAY_SIZE(bt_buff), ctx);
	limit = MODOPS_ROW_LIMIT == 0 ? size : min(size, MODOPS_ROW_LIMIT + 1);

	printk("\n\n thread %d (task %d) %c%c%c\n",
		t->id, t->task->tid,
		is_current      ? '*' : ' ',
		sched_active(t) ? 'A' : ' ',
		sched_ready(t)  ? 'R' : 'W');
	printk("Backtrace:\n"
			"    pc         func + offset\n"
			"    ---------- ------------------------\n"
			);

	for (i = is_current ? 2 : 0; i < limit; ++i) {
		printk("%3d %p", size - i, bt_buff[i]);

		s = symbol_lookup(bt_buff[i]);
		if (s != NULL) {
			offset = (char *)bt_buff[i] - (char *)s->addr;
			printk(" <%s+%#tx>", offset >= 0 ? s->name : "__unknown__",
					offset);
		}
		printk("\n");
	}
}

void whereami(void) {
	struct thread *t;
	struct task *task;

	dump_thread_stack(thread_self());

	task_foreach(task) {
		task_foreach_thread(t, task) {
			if (t != thread_self())
				dump_thread_stack(t);
		}
	}
}
