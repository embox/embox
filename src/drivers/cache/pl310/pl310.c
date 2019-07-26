/**
 * @file
 *
 * @date 28.03.2017
 * @author Anton Bondarev
 */
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <drivers/common/memory.h>
#include <kernel/time/ktime.h>
#include <hal/reg.h>
#include <util/log.h>

#include "pl310.h"

#include <embox/unit.h>
#include <framework/mod/options.h>

#define PL310_BASE    OPTION_GET(NUMBER,base_addr)

EMBOX_UNIT_INIT(pl310_init);

#define CACHE_LINE_SIZE     32

static void _reg_dump(char *s) {
	log_debug("pl310 registers: %s\n", s);
	log_debug("=====================================\n");
	log_debug("L2X0_CACHE_ID            = 0x%08x", REG32_LOAD(L2X0_CACHE_ID));
	log_debug("L2X0_CACHE_TYPE          = 0x%08x", REG32_LOAD(L2X0_CACHE_TYPE));
	log_debug("L2X0_CTRL                = 0x%08x", REG32_LOAD(L2X0_CTRL));
	log_debug("L2X0_AUX_CTRL            = 0x%08x", REG32_LOAD(L2X0_AUX_CTRL));
	log_debug("L310_TAG_LATENCY_CTRL    = 0x%08x", REG32_LOAD(L310_TAG_LATENCY_CTRL));
	log_debug("L310_DATA_LATENCY_CTRL   = 0x%08x", REG32_LOAD(L310_DATA_LATENCY_CTRL));
	log_debug("L2X0_EVENT_CNT_CTRL      = 0x%08x", REG32_LOAD(L2X0_EVENT_CNT_CTRL));
	log_debug("L2X0_EVENT_CNT1_CFG      = 0x%08x", REG32_LOAD(L2X0_EVENT_CNT1_CFG));
	log_debug("L2X0_EVENT_CNT0_CFG      = 0x%08x", REG32_LOAD(L2X0_EVENT_CNT0_CFG));
	log_debug("L2X0_EVENT_CNT1_VAL      = 0x%08x", REG32_LOAD(L2X0_EVENT_CNT1_VAL));
	log_debug("L2X0_EVENT_CNT0_VAL      = 0x%08x", REG32_LOAD(L2X0_EVENT_CNT0_VAL));
	log_debug("L2X0_INTR_MASK           = 0x%08x", REG32_LOAD(L2X0_INTR_MASK));
	log_debug("L2X0_MASKED_INTR_STAT    = 0x%08x", REG32_LOAD(L2X0_MASKED_INTR_STAT));
	log_debug("L2X0_RAW_INTR_STAT       = 0x%08x", REG32_LOAD(L2X0_RAW_INTR_STAT));
	log_debug("L2X0_INTR_CLEAR          = 0x%08x", REG32_LOAD(L2X0_INTR_CLEAR));
	log_debug("L2X0_CACHE_SYNC          = 0x%08x", REG32_LOAD(L2X0_CACHE_SYNC));
	log_debug("L2X0_DUMMY_REG           = 0x%08x", REG32_LOAD(L2X0_DUMMY_REG));
	log_debug("L2X0_INV_LINE_PA         = 0x%08x", REG32_LOAD(L2X0_INV_LINE_PA));
	log_debug("L2X0_INV_WAY             = 0x%08x", REG32_LOAD(L2X0_INV_WAY));
	log_debug("L2X0_CLEAN_LINE_PA       = 0x%08x", REG32_LOAD(L2X0_CLEAN_LINE_PA));
	log_debug("L2X0_CLEAN_LINE_IDX      = 0x%08x", REG32_LOAD(L2X0_CLEAN_LINE_IDX ));
	log_debug("L2X0_CLEAN_WAY           = 0x%08x", REG32_LOAD(L2X0_CLEAN_WAY));
	log_debug("L2X0_CLEAN_INV_LINE_PA   = 0x%08x", REG32_LOAD(L2X0_CLEAN_INV_LINE_PA));
	log_debug("L2X0_CLEAN_INV_LINE_IDX  = 0x%08x", REG32_LOAD(L2X0_CLEAN_INV_LINE_IDX));
	log_debug("L2X0_CLEAN_INV_WAY       = 0x%08x", REG32_LOAD(L2X0_CLEAN_INV_WAY));
	log_debug("L2X0_LOCKDOWN_WAY_D_BASE = 0x%08x", REG32_LOAD(L2X0_LOCKDOWN_WAY_D_BASE));
	log_debug("L2X0_LOCKDOWN_WAY_I_BASE = 0x%08x", REG32_LOAD(L2X0_LOCKDOWN_WAY_I_BASE));
	log_debug("L2X0_LOCKDOWN_STRIDE     = 0x%08x", REG32_LOAD(L2X0_LOCKDOWN_STRIDE));
	log_debug("L310_ADDR_FILTER_START   = 0x%08x", REG32_LOAD(L310_ADDR_FILTER_START));
	log_debug("L310_ADDR_FILTER_END     = 0x%08x", REG32_LOAD(L310_ADDR_FILTER_END));
	log_debug("L2X0_TEST_OPERATION      = 0x%08x", REG32_LOAD(L2X0_TEST_OPERATION ));
	log_debug("L2X0_LINE_DATA           = 0x%08x", REG32_LOAD(L2X0_LINE_DATA));
	log_debug("L2X0_LINE_TAG            = 0x%08x", REG32_LOAD(L2X0_LINE_TAG));
	log_debug("L2X0_DEBUG_CTRL          = 0x%08x", REG32_LOAD(L2X0_DEBUG_CTRL));
	log_debug("L310_PREFETCH_CTRL       = 0x%08x", REG32_LOAD(L310_PREFETCH_CTRL));
	log_debug("L310_POWER_CTRL          = 0x%08x", REG32_LOAD(L310_POWER_CTRL));
	log_debug("=====================================\n");
}

static int pl310_init(void) {
	REG_STORE(L2X0_CTRL, 0);

	REG_ORIN(L2X0_AUX_CTRL, L2X0_AUX_INSTR_PREFETCH);
	REG_ORIN(L2X0_AUX_CTRL, L2X0_AUX_DATA_PREFETCH);

	REG_STORE(L70_INVAL, 0xFFF);
	REG_STORE(PL310_BASE + 0x730, 0);

	ksleep(50);

	REG_STORE(L2X0_CTRL, 1);

	asm volatile ("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
	asm volatile ("mcr p15, 0, %0, c8, c6, 0" : : "r" (0));
	asm volatile ("mcr p15, 0, %0, c8, c5, 0" : : "r" (0));

	_reg_dump("");

	return 0;
}

PERIPH_MEMORY_DEFINE(pl310, PL310_BASE, 0x1000);
