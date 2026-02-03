#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <execinfo.h>

#include <kernel/time/sys_timer.h>
#include <kernel/printk.h>

#include <embox/unit.h>

#include <profiler/sampling/sample.h>

#include <module/embox/arch/stackframe.h>

static int hash_array[SAMPLE_HASH_SIZE];
static int pow[SAMPLE_HASH_SIZE], base = 101;
static char sample_strings[SAMPLE_HASH_SIZE][100];
//static int shift = 8;
static int shift = 0;
static bool is_running = false;
static sys_timer_t *sampling_timer;

static int get_hash(char *str) {
	/* TODO More effective hash generating algorithm */
	int res = 0, i = 0;
	while (str[i] != 0) {
		res += pow[i] * str[i];
		i++;
	}
	return res;
}

static void sampling_timer_handler(sys_timer_t* timer, void *param) {
	int i, nptrs, hash;
	void *buffer[100];
	char bt_str[64];

	nptrs = backtrace(buffer, 100);

	/* It makes sense to start not from 0 element,
	 * because the top of the callstack is always the same
	 * as we call backtrace from the same function every time
	 *
	 * Common non-informative top of the callstack looks like this:
	 *
	 * 0x001467f6 <backtrace+0x20>
	 * 0x00146860 <sampling_timer_handler+0x1f>
	 * 0x00112e54 <timer_strat_sched+0x85>
	 * 0x001793d2 <clock_handler+0xb>
	 * 0x001112fd <lthread_process+0x5e>
	 * 0x00181947 <__schedule+0x99>
	 * 0x001819a2 <sched_preempt+0x17>
	 * 0x00145a2f <critical_dispatch_pending+0x5c>
	 * 0x00100b88 <irq_handler+0x76>
	 * 0xdeadbabe <some_interesting_stuff_here+0x11>  That's where should we start
	 */

	for (i = shift; i < nptrs; i++) {
		backtrace_symbol_buf(buffer[i], bt_str, sizeof(bt_str));
		bt_str[sizeof(bt_str) - 1] = '\0';

		hash = get_hash(bt_str) % SAMPLE_HASH_SIZE;
		if (hash_array[hash] == 0) {
			strcpy(sample_strings[hash], bt_str);
		}
		hash_array[hash]++;
	}
}

static int sampling_profiler_set(int interval) {
	int res;
	interval = (interval == 0) ? (SAMPLE_TIMER_INTERVAL) : interval;

	res = sys_timer_set(&sampling_timer, SYS_TIMER_PERIODIC, interval,
		sampling_timer_handler, NULL);
	if (res) {
		printk("Failed to install timer\n");
		return res;
	}

	return ENOERR;
}


int *get_counters(void) {
	return hash_array;
}

char *get_name(int ind) {
	return sample_strings[ind];
}

bool sampling_profiler_is_running(void){
	return is_running;
}

int start_profiler(int interval) {
	int i;

	if (is_running) {
		stop_profiler();
	}

	is_running = true;
	pow[0] = 1, hash_array[0] = 0;
	for (i = 1; i < SAMPLE_HASH_SIZE; i++) {
		pow[i] = (pow[i - 1] * base) % SAMPLE_HASH_SIZE; 	/* initialize hash */
		hash_array[i] = 0;
	}
	sampling_profiler_set(interval);
	return ENOERR;
}

int stop_profiler(void) {
	is_running = false;
	sys_timer_close(sampling_timer);
	return ENOERR;
}
