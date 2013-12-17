#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <kernel/time/timer.h>
#include <kernel/printk.h>
#include <execinfo.h>
#include <embox/unit.h>

#include <profiler/sampling/sample.h>

static int hash_array[SAMPLE_HASH_SIZE];
static int pow[SAMPLE_HASH_SIZE], base = 101;
static char sample_strings[SAMPLE_HASH_SIZE][100];
static int shift = 8;
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
	char **bt_res;

	nptrs = backtrace(buffer, 100);

	bt_res = backtrace_symbols(buffer, nptrs);

	if (bt_res == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}

	/* It makes sense to start not from 0 element,
	 * because the top of the callstack is always the same
	 * as we call backtrace from the same function every time
	 *
	 * Common non-informative top of the callstack looks like this:
	 *
	 * 0x0012d66a <backtrace+0x20>
	 * 0x001035d4 <sampling_timer_handler+0x1f>
	 * 0x0012aa54 <timer_strat_sched+0x84>
	 * 0x0013c81c <soft_clock_handler+0xb>
	 * 0x0012adac <softirq_dispatch+0xa0>
	 * 0x0012b592 <critical_dispatch_pending+0x5c>
	 * 0x0010030f <irq_handler+0x76>
	 * 0x0010004c <irq_stub+0x1c>
	 * 0xdeadbabe <some_interesting_stuff_here+0x11>  That's where should we start
	 */

	for (i = shift; i < nptrs; i++) {
		hash = get_hash(bt_res[i]) % SAMPLE_HASH_SIZE;
		if (hash_array[hash] == 0) {
			strcpy(sample_strings[hash], bt_res[i]);
		}
		hash_array[hash]++;
	}
	free(bt_res);
}

static int sampling_profiler_set(void) {
	int res;
	int tick_cnt;

	tick_cnt = 0;

	if (ENOERR
			!= (res = timer_set(&sampling_timer, TIMER_PERIODIC, 100, sampling_timer_handler,
					&tick_cnt))) {
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

int start_profiler(void) {
	int i;
	is_running = true;
	pow[0] = 1;
	for (i = 1; i < SAMPLE_HASH_SIZE; i++) {
		pow[i] = (pow[i - 1] * base) % SAMPLE_HASH_SIZE; 	/* initialize hash */
		hash_array[i] = 0;
	}
	sampling_profiler_set();
	return ENOERR;
}

int stop_profiler(void) {
	timer_close(sampling_timer);
	return ENOERR;
}
