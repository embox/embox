/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <framework/mod/options.h>

#include "arch.h"
#include "breakpoint.h"

#define BREAKPOINT_COUNT OPTION_GET(NUMBER, breakpoint_count)

struct sw_breakpoint {
	void *addr;
	uint8_t orig_data[BREAK_INSTR_SZ];
};

extern void dcache_flush(const void *p, size_t size);

static struct sw_breakpoint breakpoints[BREAKPOINT_COUNT];

bool insert_breakpoint(void *addr) {
	int i;

	for (i = 0; i < BREAKPOINT_COUNT; i++) {
		if (breakpoints[i].addr == NULL) {
			memcpy(&breakpoints[i].orig_data, addr, BREAK_INSTR_SZ);
			memcpy(addr, break_instr, BREAK_INSTR_SZ);
			dcache_flush(addr, BREAK_INSTR_SZ);
			breakpoints[i].addr = addr;
			break;
		}
	}
	return (i != BREAKPOINT_COUNT);
}

bool remove_breakpoint(void *addr) {
	int i;

	for (i = 0; i < BREAKPOINT_COUNT; i++) {
		if (breakpoints[i].addr == addr) {
			memcpy(addr, &breakpoints[i].orig_data, BREAK_INSTR_SZ);
			dcache_flush(addr, BREAK_INSTR_SZ);
			breakpoints[i].addr = NULL;
			break;
		}
	}
	return (i != BREAKPOINT_COUNT);
}

void remove_all_breakpoints(void) {
	void *addr;
	int i;

	for (i = 0; i < BREAKPOINT_COUNT; i++) {
		if (breakpoints[i].addr != NULL) {
			addr = breakpoints[i].addr;
			memcpy(addr, &breakpoints[i].orig_data, BREAK_INSTR_SZ);
			dcache_flush(addr, BREAK_INSTR_SZ);
			breakpoints[i].addr = NULL;
		}
	}
}
