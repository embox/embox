/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 * @author Denis Deryugin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <hal/context.h>

#include <lib/libds/array.h>
#include <util/location.h>
#include <lib/libds/hashtable.h>

#include <mem/misc/pool.h>

#include <debug/symbol.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <kernel/time/itimer.h>

#include <profiler/tracing/trace.h>

#include <embox/unit.h>
#include "cyg_profile.h"

EMBOX_UNIT_INIT(instrument_profiling_init);

ARRAY_SPREAD_DEF_TERMINATED(struct __trace_point *,
		__trace_points_array, NULL);
ARRAY_SPREAD_DEF_TERMINATED(struct __trace_block *,
		__trace_blocks_array, NULL);

POOL_DEF(tb_pool, struct __trace_block, FUNC_QUANTITY);
POOL_DEF(itimer_pool, struct itimer, FUNC_QUANTITY);
POOL_DEF(key_pool, int, FUNC_QUANTITY);
POOL_DEF(st_pool, struct tb_time, TB_MAX_DEPTH);

POOL_DEF(tb_ht_pool, struct hashtable_item, FUNC_QUANTITY);

/* This variable contains current profiling mode (see profiler/tracing/trace.h) */
static profiling_mode p_mode;
profiling_mode get_profiling_mode(void) {
	return p_mode;
}
void set_profiling_mode(profiling_mode new_mode) {
	p_mode = new_mode;
	return;
}

void cyg_tracing_profiler_enter(void *func, void *caller) {
	/* This function is for instrument profiling. It is being called just before
	 * every call of instrumented funcion.
	 * You can try to get more info by searching for "-finstrument-functions" GCC flag
	 */
	if (get_profiling_mode() == CYG_PROFILING) {
		set_profiling_mode(DISABLED);
		trace_block_func_enter(func);
		set_profiling_mode(CYG_PROFILING);
	}
}

void cyg_tracing_profiler_exit(void *func, void *caller) {
	/* This function is for instrument profiling. It is being called after every
	 * exit from instrumented funcion.
	 * You can try to get more info by searching for "-finstrument-functions" GCC flag
	 */
	 if (get_profiling_mode() == CYG_PROFILING) {
		set_profiling_mode(DISABLED);
		trace_block_func_exit(func);
		set_profiling_mode(CYG_PROFILING);
	}
}

/* Functions for hash */

static size_t get_trace_block_hash(void *key) {
	/* trace_block hash is just the pointer, beacuse
	 * it is unique for every functions, and there must be
	 * one block for one funcion.
	 */
	return (size_t) key;
}

static int cmp_trace_blocks(void *key1, void *key2) {
	/* trace_block hash is just the pointer, beacuse
	 * it is unique for every functions, and there must be
	 * one block for one funcion.
	 * So it is enough to compare those pointers.
	 */

	return 1 - (key1 == key2);
}

/* Hashtable to keep all dynamically generated trace_blocks */
HASHTABLE_DEF(tbhash_full,
		FUNC_QUANTITY * sizeof(struct __trace_block),
		get_trace_block_hash,
		cmp_trace_blocks);

static struct hashtable *tbhash = &tbhash_full;

static int **prev_key;

/* Global clock_source to keep all trace_block consistent */
static struct clock_source *tb_cs = NULL;

void __tracepoint_handle(struct __trace_point *tp) {
	if (tp->active) {
		tp->count++;
	}
}

void trace_block_enter(struct __trace_block *tb) {
	/* Necessary actions to capture information on trace_block enter */
	time64_t cur_time;
	struct tb_time *t;
	if (tb->active && tb_cs) {
		tb->count++;
		tb->depth++;
		cur_time = tb_cs->counter_device->read(tb_cs);

		t = (struct tb_time*) pool_alloc (&st_pool);
		if (t) {assert(t != NULL);
		t->next = tb->time_list_head;
		t->time = cur_time;
		tb->time_list_head = t; }
	}
}

void trace_block_leave(struct __trace_block *tb) {
	/* Necessary actions to capture information on trace_block exit */
	time64_t cur_time;
	struct tb_time *p;
	if (tb->active && tb_cs) {

		/* When cyg_profiling was enabled during trace_block,
		 * tb_leave may occur without tb_enter */
		if (tb->depth == 0)
			return;

		tb->depth--;

		cur_time = tb_cs->counter_device->read(tb_cs);

		p = tb->time_list_head;
		if (p) {
		assert(p != NULL);

		cur_time -= tb->time_list_head->time;
		cur_time = cur_time > 0 ? cur_time : 0;
		tb->time += cur_time;

		if (tb->max_time < cur_time)
			tb->max_time = cur_time;

		tb->time_list_head = p->next;
		pool_free(&st_pool, p); }
	}
}

time64_t trace_block_get_time(struct __trace_block *tb) {
	return 0;
	//return tb->time;
}

int trace_point_get_value(struct __trace_point *tp) {
	return tp->count;
}

struct __trace_point *trace_point_get_by_name(const char *name) {
	struct __trace_point *tp;

	array_spread_nullterm_foreach(tp, __trace_points_array)
	{
		if (!strcmp(tp->name, name)) {
			return tp;
		}
	}

	return NULL;
}

/* Functions for debug output */

void print_trace_block_info(struct __trace_block *tb) {
	/* Printing information about trace_block */
	printf("Trace block info: %p %s\n", tb, tb->name);
	printf("Time counter pointer:\n");
	printf("Active: %s\nIs entered: %s\n", tb->active ? "YES" : "NO",
											tb->is_entered ? "YES" : "NO");
}

/* It is assumed that there are traceblocks for every function
 * with trace_block_enter just after function call and
 * trace_block_exit just before function exit */

void trace_block_func_enter(void *func) {
	/* Function for auto-generated trace_blocks
	 * It has to find appropriate trace_block in hash_table
	 * or to generate new one
	 *	Args: void *func - pointer to functions that is about to enter
	 */
	struct __trace_block *tb = NULL;

	tb = hashtable_get(tbhash, func);
	if (!tb) {
		struct hashtable_item *ht_item;
		/* Lazy traceblock initialization.
		 * Func name and func location will be retrieved somewhere else,
		 * for example, in "trace_blocks -n" shell command. */

		tb = (struct __trace_block*) pool_alloc (&tb_pool);

		tb->func = func;
		tb->time = tb->max_time = tb->count = tb->depth = 0;
		tb->time_list_head = NULL;
		tb->active = true;
		tb->is_entered = false;


		ht_item = pool_alloc(&tb_ht_pool);
		ht_item = hashtable_item_init(ht_item, func, tb);
		hashtable_put(tbhash, ht_item);
	}

	trace_block_enter(tb);
}

void trace_block_func_exit(void *func) {
	/* Function for auto-generated trace_blocks
	 * It has to do all changes according to trace_block_leave
	 *	Args: void *func - pointer to functions that just exited
	 */
	struct __trace_block *tb;
	if (!tbhash) {
		return;
	}
	tb = hashtable_get(tbhash, func);
	if (tb) {
		trace_block_leave(tb);
	}
}

struct __trace_block *auto_profile_tb_first(void){
	/* Get the pointer to the first trace_block in trace_block hash table */
	if (!tbhash)
		return NULL;
	prev_key = hashtable_get_key_first(tbhash);
	return (struct __trace_block *) hashtable_get(tbhash, *prev_key);
}


struct __trace_block *auto_profile_tb_next(struct __trace_block *prev){
	/* Getting the pointer to the next trace_block
	 *	Args:	*prev - pointer to previous trace_block
	 */
	prev_key = hashtable_get_key_next(tbhash, prev_key);
	return (struct __trace_block *) hashtable_get(tbhash, *prev_key);
}

static int instrument_profiling_init(void) {
	set_profiling_mode(DISABLED);

	tb_cs = clock_source_get_best(CS_WITHOUT_IRQ);

	ARRAY_SPREAD_DECLARE(cyg_func, __cyg_handler_enter_array);
	ARRAY_SPREAD_DECLARE(cyg_func, __cyg_handler_exit_array);
	ARRAY_SPREAD_ADD(__cyg_handler_enter_array, &cyg_tracing_profiler_enter);
	ARRAY_SPREAD_ADD(__cyg_handler_exit_array, &cyg_tracing_profiler_exit);

	return 0;
}

long long get_current_tb_resolution(void) {
	if (tb_cs && tb_cs->counter_device)
		return tb_cs->counter_device->cycle_hz;
	else
		return 1;
}

void trace_block_hashtable_init(void) {
	/* Initializing trace_block hash table */
	profiling_mode c = get_profiling_mode();
	struct __trace_block *tb1, *tb2;
	struct hashtable_item *ht_item;

	set_profiling_mode(DISABLED);

	tb1 = auto_profile_tb_first();
	while (tb1) {
		pool_free(&tb_pool, tb1);
		tb2 = tb1;
		tb1 = auto_profile_tb_next(tb1);
		ht_item = hashtable_del(tbhash, tb2->func);
		pool_free(&tb_ht_pool, ht_item);
	}

	tb_cs = clock_source_get_best(CS_WITHOUT_IRQ);

	set_profiling_mode(c);
}


