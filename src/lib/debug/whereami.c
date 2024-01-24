/**
 * @file
 * @brief
 *
 * @date 10.10.13
 * @author Ilia Vaprol
 */

#include <stdio.h>

#include <execinfo.h>

#include <debug/symbol.h>
#include <debug/whereami.h>
#include <kernel/printk.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched.h>
#include <hal/context.h>
#include <lib/libds/array.h>
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

	lineno_len = tb_decimal_width(loc->line) + 1;  /* including a colon */
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

	if (t == NULL) {
		p += tb_safe_snprintf(p, end-p,
				" -- Current thread is NULL, it seems that kernel"
				" was not initialized yet ");
	} else {
		p += tb_safe_snprintf(p, end-p,
				" --   %08x %c %c %c %c  thread %d  task %d ",
				t->critical_count,
				is_current      ? '*' : ' ',
				sched_active(&t->schedee) ? 'A' : ' ',
				t->schedee.ready        ? 'R' : ' ',
				t->schedee.waiting      ? 'W' : ' ',
				t->id, task_get_id(t->task));
	}

	memset(p, '-', end-p-1);
	*(end-1) = '\0';

	return buff_sz;
}

static void do_traceback_dump_thread(struct thread *t, int skip) {
	struct context *ctx;
	int is_current;
	int size, limit;
	char buff[ROW_SZ];

	if (!t)
		t = thread_self();
	is_current = (t == thread_self());

	ctx  = is_current ? NULL : &t->context;
	size = backtrace_context(&bt_buff[0], ARRAY_SIZE(bt_buff), ctx);
	limit = MAX_ROWS ? min(size, MAX_ROWS + 1) : size;

	tb_snprint_thread_state(buff, sizeof(buff), t);
	printk("\n\n%s\n\n", buff);

	for (int i = skip; i < limit; ++i) {
		tb_snprint_stack_frame(buff, sizeof(buff),
			size-i, bt_buff[i]);
		printk("%s\n", buff);
	}

	tb_snprint_thread_run(buff, sizeof(buff), t);
	printk("\n%s\n", buff);
}

void traceback_dump_thread(struct thread *t) {
	do_traceback_dump_thread(t, (t == thread_self()) ? 1 : 0);
}

void traceback_dump(void) {
	do_traceback_dump_thread(thread_self(), 2);
}

void whereami(void) {
	struct thread *t;
	struct task *task;

	do_traceback_dump_thread(thread_self(), 2);

	task_foreach(task) {
		task_foreach_thread(t, task) {
			if (t != thread_self())
				do_traceback_dump_thread(t, 0);
		}
	}
}
