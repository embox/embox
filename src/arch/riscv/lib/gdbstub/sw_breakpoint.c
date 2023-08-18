/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <asm/cache.h>

#include <framework/mod/options.h>

#define SW_BPT_COUNT OPTION_GET(NUMBER, sw_breakpoint_count)

#ifdef __riscv_c
static const uint16_t bpt_instr = 0x9002; /* c.ebreak */
#else
static const uint32_t bpt_instr = 0x00100073; /* ebreak */
#endif

struct sw_breakpoint {
	void *addr;
	uint8_t orig[sizeof(bpt_instr)];
};

static bool sw_bpts_enabled = false;
static bool sw_bpts_activated = false;

static struct sw_breakpoint sw_bpts[SW_BPT_COUNT];

bool riscv_set_sw_bpt(void *addr) {
	int i;

	if (!sw_bpts_enabled) {
		return false;
	}

	for (i = 0; i < SW_BPT_COUNT; i++) {
		if (sw_bpts[i].addr == NULL) {
			sw_bpts[i].addr = addr;
			memcpy(&sw_bpts[i].orig, addr, sizeof(bpt_instr));
			if (sw_bpts_activated) {
				memcpy(addr, &bpt_instr, sizeof(bpt_instr));
			}
			break;
		}
	}

	return (i != SW_BPT_COUNT);
}

bool riscv_remove_sw_bpt(void *addr) {
	int i;

	if (!sw_bpts_enabled) {
		return false;
	}

	for (i = 0; i < SW_BPT_COUNT; i++) {
		if (sw_bpts[i].addr == addr) {
			sw_bpts[i].addr = NULL;
			if (sw_bpts_activated) {
				memcpy(addr, &sw_bpts[i].orig, sizeof(bpt_instr));
			}
			break;
		}
	}

	return (i != SW_BPT_COUNT);
}

void riscv_remove_sw_bpts(void) {
	int i;

	if (!sw_bpts_enabled) {
		return;
	}

	for (i = 0; i < SW_BPT_COUNT; i++) {
		if (sw_bpts[i].addr != NULL) {
			if (sw_bpts_activated) {
				memcpy(sw_bpts[i].addr, &sw_bpts[i].orig, sizeof(bpt_instr));
			}
			sw_bpts[i].addr = NULL;
		}
	}
}

void riscv_activate_sw_bpts(void) {
	int i;

	if (!sw_bpts_enabled) {
		return;
	}

	for (i = 0; i < SW_BPT_COUNT; i++) {
		if (sw_bpts[i].addr != NULL) {
			memcpy(sw_bpts[i].addr, &bpt_instr, sizeof(bpt_instr));
		}
	}

	riscv_icache_flush();

	sw_bpts_activated = true;
}

void riscv_deactivate_sw_bpts(void) {
	int i;

	if (!sw_bpts_enabled) {
		return;
	}

	for (i = 0; i < SW_BPT_COUNT; i++) {
		if (sw_bpts[i].addr != NULL) {
			memcpy(sw_bpts[i].addr, &sw_bpts[i].orig, sizeof(bpt_instr));
		}
	}

	riscv_icache_flush();

	sw_bpts_activated = false;
}

void riscv_enable_sw_bpts(void) {
	sw_bpts_enabled = true;
}

void riscv_disable_sw_bpts(void) {
	sw_bpts_enabled = false;
}
