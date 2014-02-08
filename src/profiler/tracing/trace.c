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

#include <util/array.h>
#include <util/location.h>
#include <util/hashtable.h>

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

static struct hashtable *tbhash = NULL;
static char **prev_key = NULL;

void __tracepoint_handle(struct __trace_point *tp) {
	if (tp->active) {
		tp->count++;
	}
}

void trace_block_enter(struct __trace_block *tb) {
	if (tb->active) {
		tb->is_entered = true;
		itimer_init(tb->tc, clock_source_get_best(CS_WITHOUT_IRQ), 0);
		/*__tracepoint_handle(tb->begin);*/
	}
}

void trace_block_leave(struct __trace_block *tb) {
	if (tb->active) {
		tb->count++;
		tb->is_entered = false;
		tb->time = itimer_read(tb->tc);
		/* __tracepoint_handle(tb->end); */
	}
}

time64_t trace_block_get_time(struct __trace_block *tb) {
	return tb->time;
}

time64_t trace_block_diff(struct __trace_block *tb) {
	time64_t a = trace_point_get_value(tb->begin);
	time64_t b = trace_point_get_value(tb->end);

	return a - b;
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
	printf("Trace block info: %p %s\n", tb, tb->name);
	printf("Time counter pointer: %p\n", tb->tc);
	printf("Active: %s\nIs entered: %s\n", tb->active ? "YES" : "NO",
											tb->is_entered ? "YES" : "NO");
}

/* Functions for hash */

static int str_hash(const char *c){
	int s = 0, i = 0;
	while (c[i]) {
		s += c[i++];
	}
	return s;
}

static size_t get_trace_block_hash(void *key) {
	return str_hash(((char*) key));
}

static int cmp_trace_blocks(void *key1, void *key2) {
	return strcmp((char*)key1, (char*)key2);
}

/* It is assumed that there are traceblocks for every function
 * with trace_block_enter just after function call and
 * trace_block_exit just before function exit */

void __cyg_profile_func_enter(void *func, void *caller) {
	/* TODO: get function name and function location*/
	char *name = NULL, location[] = "FUNCTION_LOCATION";
	struct __trace_block *tb = NULL;

	name = (char*) malloc (sizeof(char) * 20);
	strcpy(name, "FUNCTION_NAME");

	tb = hashtable_get(tbhash, name);

	if (!tb) {
		/* Lazy traceblock initialization */
		tb = (struct __trace_block*) malloc (sizeof(struct __trace_block));
		tb->name = name;
		tb->tc = (struct itimer *) malloc (sizeof(struct itimer)),
		tb->time = 0;
		tb->count = 0;
		tb->active = true;
		tb->is_entered = false;
		tb->location = (struct location_func) {
			.at = {"FILE", 0},
			.func = location
		};
		hashtable_put(tbhash, name, tb);
	}
	tb = hashtable_get(tbhash, name);
	trace_block_enter(tb);
}

void __cyg_profile_func_exit(void *func, void *caller) {
	/* TODO: get function name and function location*/
	char name[] = "FUNCTION_NAME";
	struct __trace_block *tb;
	tb = hashtable_get(tbhash, name);
	trace_block_leave(tb);
}

struct __trace_block *auto_profile_tb_first(void){
	prev_key = hashtable_get_key_first(tbhash);
	return (struct __trace_block *) hashtable_get(tbhash, *prev_key);
}

struct __trace_block *auto_profile_tb_next(struct __trace_block *prev){
	prev_key = hashtable_get_key_next(tbhash, &prev_key);
	return (struct __trace_block *) hashtable_get(tbhash, *prev_key);
}

static int instrument_profiling_init(void) {
	/* Initializing hash table */
	tbhash = hashtable_create(100 * sizeof(struct __trace_block),
				get_trace_block_hash, cmp_trace_blocks);

	if (!tbhash) {
		return 1;
		fprintf(stderr, "Unable to create hashtable for profiling\n");
	}
	return 0;
}
