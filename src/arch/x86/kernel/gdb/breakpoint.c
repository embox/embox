/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <framework/mod/options.h>

#define MAX_BREAKPOINTS OPTION_GET(NUMBER, max_breakpoints)

extern void dcache_flush(const void *p, size_t size);

static const uint8_t bpt_instr = 0xCC;

static struct {
	void *addr;
	uint8_t orig;
} bpts[MAX_BREAKPOINTS];

int gdb_set_bpt(void *addr) {
	int i;

	for (i = 0; i < MAX_BREAKPOINTS; i++) {
		if (bpts[i].addr == NULL) {
			memcpy(&bpts[i].orig, addr, sizeof(bpt_instr));
			memcpy(addr, &bpt_instr, sizeof(bpt_instr));
			dcache_flush(addr, sizeof(bpt_instr));
			bpts[i].addr = addr;
			break;
		}
	}
	return (i != MAX_BREAKPOINTS);
}

int gdb_remove_bpt(void *addr) {
	int i;

	for (i = 0; i < MAX_BREAKPOINTS; i++) {
		if (bpts[i].addr == addr) {
			memcpy(addr, &bpts[i].orig, sizeof(bpt_instr));
			dcache_flush(addr, sizeof(bpt_instr));
			bpts[i].addr = NULL;
			break;
		}
	}
	return (i != MAX_BREAKPOINTS);
}

void gdb_remove_all_bpts(void) {
	int i;

	for (i = 0; i < MAX_BREAKPOINTS; i++) {
		if (bpts[i].addr != NULL) {
			memcpy(bpts[i].addr, &bpts[i].orig, sizeof(bpt_instr));
			dcache_flush(bpts[i].addr, sizeof(bpt_instr));
			bpts[i].addr = NULL;
		}
	}
}
