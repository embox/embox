/**
 * @file
 * @brief
 *
 * @date 10.10.13
 * @author Ilia Vaprol
 */

#include <execinfo.h>
#include <stdio.h>

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

#define MAX_ROWS OPTION_GET(NUMBER, row_limit)
#define MAX_DEPTH OPTION_GET(NUMBER, max_depth)

#define ROW_SZ 100

#define ELLI "..."

static void *bt_buff[MAX_DEPTH];

#define tb_safe_snprintf(p, sz, fmt, ...) \
	({ ptrdiff_t __sz = sz; assert(__sz >= 0); \
		__sz ? min(snprintf(p, __sz, fmt, ##__VA_ARGS__), __sz-1) : 0; })

static size_t tb_snprint_stack_frame(char *buff, size_t buff_sz,
		int nr, void *addr, const struct symbol *sym) {
	char *p = buff;
	char *end = buff + buff_sz;

	p += tb_safe_snprintf(p, end-p, "%3d %p", nr, addr);

	if (sym) {
		const struct location *loc = &sym->loc;
		ptrdiff_t offset = (char *) addr - (char *) sym->addr;
		p += tb_safe_snprintf(p, end-p, " <%s+%#tx> ",
			(offset < 0 ? "__unknown__" : sym->name), offset);

		if (loc->file) {
			const char *fname = loc->file;
			size_t lineno_len = 1;  /* including a colon */
			int lineno_div = loc->line;
			do {
				++lineno_len;
			} while ((lineno_div /= 10));

			if (end-p > strlen(ELLI) + lineno_len) {
				size_t fname_len = strlen(fname);
				int padding = (end-p) - (fname_len + lineno_len + 1);

				if (padding < 0) {
					fname += strlen(ELLI) - padding;
					p += tb_safe_snprintf(p, end-p, "%s", ELLI);
				} else {
					for (int i = 0; i < padding; ++i)
						p += tb_safe_snprintf(p, end-p, " ");
				}

				p += tb_safe_snprintf(p, end-p, "%s:%d", fname, loc->line);
			}
		}
	}

	return end-p;
}

static void tb_dump_thread_stack(struct thread *t) {
	struct context *ctx;
	int size, limit;
	int is_current = (t == thread_self());
	char buff[ROW_SZ];

	ctx  = is_current ? NULL : &t->context;
	size = backtrace_context(&bt_buff[0], ARRAY_SIZE(bt_buff), ctx);
	limit = MAX_ROWS ? min(size, MAX_ROWS + 1) : size;

	printk("\n\n thread %d (task %d) %c%c%c\n",
		t->id, t->task->tid,
		is_current      ? '*' : ' ',
		sched_active(t) ? 'A' : ' ',
		sched_ready(t)  ? 'R' : 'W');
	printk("Backtrace:\n"
			"    pc         func + offset\n"
			"    ---------- ------------------------\n"
			);

	for (int i = is_current ? 2 : 0; i < limit; ++i) {
		tb_snprint_stack_frame(buff, sizeof(buff),
			size-i, bt_buff[i], symbol_lookup(bt_buff[i]));
		printk("%s\n", buff);
	}
}

void whereami(void) {
	struct thread *t;
	struct task *task;

	tb_dump_thread_stack(thread_self());

	task_foreach(task) {
		task_foreach_thread(t, task) {
			if (t != thread_self())
				tb_dump_thread_stack(t);
		}
	}
}
