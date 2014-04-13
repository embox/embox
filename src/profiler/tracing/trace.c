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

#include <util/array.h>
#include <util/location.h>
#include <util/hashtable.h>

#include <mem/misc/pool.h>

#include <debug/symbol.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <kernel/time/itimer.h>

#include <profiler/tracing/trace.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(instrument_profiling_init);

ARRAY_SPREAD_DEF_TERMINATED(struct __trace_point *,
		__trace_points_array, NULL);
ARRAY_SPREAD_DEF_TERMINATED(struct __trace_block *,
		__trace_blocks_array, NULL);

POOL_DEF(tb_pool, struct __trace_block, FUNC_QUANTITY);
POOL_DEF(itimer_pool, struct itimer, FUNC_QUANTITY);
POOL_DEF(key_pool, int, FUNC_QUANTITY);

int cyg_profiling;

/* Hashtable to keep all dynamically generated trace_blocks */
static struct hashtable *tbhash = NULL;
static int **prev_key;

/* Global clock_source to keep all trace_block consistent */
static struct clock_source *tb_cs = NULL;

void __tracepoint_handle(struct __trace_point *tp) {
	if (tp->active) {
		tp->count++;
	}
}

void trace_block_enter(struct __trace_block *tb) {
	if (tb->active && tb_cs) {
		tb->is_entered = true;
		tb->count++;
		tb->start = tb_cs->counter_device->read();
		//itimer_init(tb->tc, clock_source_get_best(CS_WITHOUT_IRQ), 0);
	}
}

void trace_block_leave(struct __trace_block *tb) {
	if (tb->active && tb_cs) {
		tb->is_entered = false;
		tb->time += abs(tb_cs->counter_device->read() - tb->start);
		//tb->time += itimer_read(tb->tc);
	}
}

time64_t trace_block_get_time(struct __trace_block *tb) {
	return tb->time;
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

/* Functions for hash */

/*static int str_hash(const char *c){
	int s = 0, i = 0;
	while (c[i]) {
		s += c[i++];
	}
	return s;
}*/

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
	if (!tbhash) {
		/* Table is not initialized */
		return;
	}
	tb = hashtable_get(tbhash, func);

	if (!tb) {
		/* Lazy traceblock initialization.
		 * Func name and func location will be retrieved somewhere else,
		 * for example, in "trace_blocks -n" shell command.
		 */

		tb = (struct __trace_block*) pool_alloc (&tb_pool);

		tb->func = func;
		tb->start = 0;
		tb->time = 0;
		tb->count = 0;
		tb->active = true;
		tb->is_entered = false;
		hashtable_put(tbhash, func, tb);
	} else {
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
	cyg_profiling = false;
	tbhash = NULL;
	tb_cs = clock_source_get_best(CS_WITHOUT_IRQ);
	/* Initializing hash table */
	tbhash = hashtable_create(FUNC_QUANTITY * sizeof(struct __trace_block),
				get_trace_block_hash, cmp_trace_blocks);

	if (!tbhash) {
		return -ENOMEM;
		fprintf(stderr, "Unable to create hashtable for profiling\n");
	}
	return 0;
}

void trace_block_hashtable_init(void) {
	/* Initializing trace_block hash table */
	int c = cyg_profiling;
	cyg_profiling = false;

	if (tbhash) {
		cyg_profiling = c;
		return;
	}
	/*tbhash = hashtable_create(FUNC_QUANTITY * sizeof(struct __trace_block),
				get_trace_block_hash, cmp_trace_blocks);
	*/
	cyg_profiling = c;
}
void trace_block_hashtable_destroy(void) {
	/* Clear trace_block hash table */
	int c = cyg_profiling;
	cyg_profiling = false;

	if (tbhash)
		hashtable_destroy(tbhash);
	tbhash = NULL;
	cyg_profiling = c;
}

