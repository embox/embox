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

#define MAX_ROWS  OPTION_GET(NUMBER, max_rows)
#define MAX_DEPTH OPTION_GET(NUMBER, max_depth)
#define ROW_SZ    OPTION_GET(NUMBER, row_sz)

#define ELLI "..."

static void *bt_buff[MAX_DEPTH];

#define tb_safe_snprintf(p, sz, fmt, ...) \
	({ ptrdiff_t __sz = sz; assert(__sz >= 0); \
		__sz ? min(snprintf(p, __sz, fmt, ##__VA_ARGS__), __sz-1) : 0; })

static inline int tb_decimal_width(int i) {
	char buff[11];
	return snprintf(buff, sizeof(buff), "%d", i);
}

static size_t tb_snprint_symbol(char *buff, size_t buff_sz, void *addr) {
	char *p = buff;
	char *end = buff + buff_sz;
	const struct symbol *sym;
	const struct location *loc;

	size_t lineno_len;  /* including a colon */
	const char *fname;
	size_t fname_len;
	int fname_padding;

	p += tb_safe_snprintf(p, end-p, "%p", addr);

	sym = symbol_lookup(addr);
	if (!sym)
		goto out;

	p += tb_safe_snprintf(p, end-p, " <%s+%#tx> ",
		sym->name, (char *) addr - (char *) sym->addr);

	loc = &sym->loc;
	if (!loc->file)
		goto out;

	lineno_len = tb_decimal_width(loc->line);
	if (end-p <= strlen(ELLI) + lineno_len)
		goto out;

	fname = loc->file;
	fname_len = strlen(fname);
	fname_padding = (end-p) - (fname_len + lineno_len + 1);

	if (fname_padding < 0) {
		fname += strlen(ELLI) - fname_padding;
		p += tb_safe_snprintf(p, end-p, "%s", ELLI);
	} else {
		for (int i = 0; i < fname_padding; ++i)
			p += tb_safe_snprintf(p, end-p, " ");
	}

	p += tb_safe_snprintf(p, end-p, "%s:%d", fname, loc->line);

out:
	return end-p;
}

static size_t tb_snprint_stack_frame(char *buff, size_t buff_sz,
		int nr, void *addr) {
	char *p = buff;
	char *end = buff + buff_sz;

	p += tb_safe_snprintf(p, end-p, "%3d ", nr);
	return tb_snprint_symbol(p, end-p, addr);
}

static size_t tb_snprint_thread_run(char *buff, size_t buff_sz,
		struct thread *t) {
	char *p = buff;
	char *end = buff + buff_sz;

	p += tb_safe_snprintf(p, end-p, "run ");
	return tb_snprint_symbol(p, end-p, t->run);
}

static size_t tb_snprint_thread_state(char *buff, size_t buff_sz,
		struct thread *t) {
	char *p = buff;
	char *end = buff + buff_sz;
	int is_current = (t == thread_self());

	p += tb_safe_snprintf(p, end-p,
		" ==    %c %c %c %c thread %3d  (task %2d) ",
		is_current      ? '*' : ' ',
		sched_active(t) ? 'A' : ' ',
		t->ready        ? 'R' : ' ',
		t->waiting      ? 'W' : ' ',
		t->id, t->task->tid);

	memset(p, '=', end-p-1);
	*(end-1) = '\0';

	return buff_sz;
}

static void tb_dump_thread_stack(struct thread *t) {
	struct context *ctx;
	int size, limit;
	int is_current = (t == thread_self());
	char buff[ROW_SZ];

	ctx  = is_current ? NULL : &t->context;
	size = backtrace_context(&bt_buff[0], ARRAY_SIZE(bt_buff), ctx);
	limit = MAX_ROWS ? min(size, MAX_ROWS + 1) : size;

	tb_snprint_thread_state(buff, sizeof(buff), t);
	printk("\n\n%s\n", buff);

	printk("    pc         func + offset\n");
	printk("    ---------- ------------------------\n");

	for (int i = is_current ? 2 : 0; i < limit; ++i) {
		tb_snprint_stack_frame(buff, sizeof(buff),
			size-i, bt_buff[i]);
		printk("%s\n", buff);
	}

	tb_snprint_thread_run(buff, sizeof(buff), t);
	printk("\n%s\n", buff);
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
