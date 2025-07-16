/**
 * @file
 *
 * @date Mar 11, 2020
 * @author Anton Bondarev
 */

#include <stdbool.h>
#include <stdint.h>

#include <asm/cacheops.h>
#include <asm/io.h>
#include <asm/mipsregs.h>
#include <asm/system.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <kernel/panic.h>
#include <util/log.h>

#define USE_L2_CACHE        OPTION_GET(BOOLEAN, use_l2_cache)
#define USE_CACHE_SIZE_AUTO OPTION_GET(BOOLEAN, use_auto_cache_size)

EMBOX_UNIT_INIT(mips_cache_init);

#ifndef CONFIG_SYS_ICACHE_LINE_SIZE
#define CONFIG_SYS_ICACHE_LINE_SIZE	32
#endif

#ifndef CONFIG_SYS_DCACHE_LINE_SIZE
#define CONFIG_SYS_DCACHE_LINE_SIZE	32
#endif

#ifndef CONFIG_SYS_SCACHE_LINE_SIZE
#define CONFIG_SYS_SCACHE_LINE_SIZE	0
#endif

#if USE_L2_CACHE
extern uint32_t mips_cm_l2_line_size(void);

static uint32_t l2_line_size = CONFIG_SYS_SCACHE_LINE_SIZE;
#endif

#if USE_CACHE_SIZE_AUTO
static uint32_t l1i_line_size = CONFIG_SYS_ICACHE_LINE_SIZE;
static uint32_t l1d_line_size = CONFIG_SYS_DCACHE_LINE_SIZE;
#endif

static inline unsigned long icache_line_size(void) {
#if USE_CACHE_SIZE_AUTO
	return l1i_line_size;
#else
	return CONFIG_SYS_ICACHE_LINE_SIZE;
#endif
}

static inline unsigned long dcache_line_size(void) {
#if USE_CACHE_SIZE_AUTO
	return l1d_line_size;
#else
	return CONFIG_SYS_DCACHE_LINE_SIZE;
#endif
}

static inline unsigned long scache_line_size(void) {
#if USE_L2_CACHE
	return l2_line_size;
#else
	return CONFIG_SYS_SCACHE_LINE_SIZE;
#endif
}

#if USE_L2_CACHE
static void probe_l2(void) {
	unsigned long conf2, sl;
	bool l2c = false;

	if (!(mips_read_c0_config1() & MIPS_CONF_M))
		return;

	conf2 = mips_read_c0_config2();

	if (__mips_isa_rev >= 6) {
		l2c = conf2 & MIPS_CONF_M;
		if (l2c)
			l2c = mips_read_c0_config3() & MIPS_CONF_M;
		if (l2c)
			l2c = mips_read_c0_config4() & MIPS_CONF_M;
		if (l2c)
			l2c = mips_read_c0_config5() & MIPS_CONF5_L2C;
	}

	if (l2c) {
		l2_line_size = mips_cm_l2_line_size();
	}
	else if (l2c) {
		/* We don't know how to retrieve L2 config on this system */
		panic("We don't know how to retrieve L2 config on this system");
	}
	else {
		sl = (conf2 & MIPS_CONF2_SL) >> MIPS_CONF2_SL_SHF;
		l2_line_size = sl ? (2 << sl) : 0;
	}
}
#endif

static int mips_cache_init(void) {
#if USE_CACHE_SIZE_AUTO
	unsigned long conf1, il, dl;

	conf1 = mips_read_c0_config1();

	il = (conf1 & MIPS_CONF1_IL) >> MIPS_CONF1_IL_SHF;
	dl = (conf1 & MIPS_CONF1_DL) >> MIPS_CONF1_DL_SHF;

	l1i_line_size = il ? (2 << il) : 0;
	l1d_line_size = dl ? (2 << dl) : 0;
#endif

	log_info("cache L1: instr line size (%d) data line size (%d)\n",
	    icache_line_size(), dcache_line_size());
#if USE_L2_CACHE
	probe_l2();
	log_info("cache L2: line size (%d)\n", scache_line_size());
#endif
	return 0;
}

#define cache_loop(start, end, lsize, ops...)                          \
	do {                                                               \
		const void *addr = (const void *)(start & ~(lsize - 1));       \
		const void *aend = (const void *)((end - 1) & ~(lsize - 1));   \
		const unsigned int cache_ops[] = {ops};                        \
		unsigned int i;                                                \
                                                                       \
		if (!lsize) {                                                  \
			break;                                                     \
		}                                                              \
		for (; addr <= aend; addr += lsize) {                          \
			for (i = 0; i < ARRAY_SIZE(cache_ops); i++)                \
				mips_cache(cache_ops[i], (const volatile void *)addr); \
		}                                                              \
	} while (0)

void flush_cache(unsigned long start_addr, size_t size) {
	unsigned long ilsize = icache_line_size();
	unsigned long dlsize = dcache_line_size();
	unsigned long slsize = scache_line_size();

	/* aend will be miscalculated when size is zero, so we return here */
	if (size == 0) {
		return;
	}

	if ((ilsize == dlsize) && !slsize) {
		/* flush I-cache & D-cache simultaneously */
		cache_loop(start_addr, start_addr + size, ilsize, HIT_WRITEBACK_INV_D,
		    HIT_INVALIDATE_I);
		goto ops_done;
	}

	/* flush D-cache */
	cache_loop(start_addr, start_addr + size, dlsize, HIT_WRITEBACK_INV_D);

	/* flush L2 cache */
	cache_loop(start_addr, start_addr + size, slsize, HIT_WRITEBACK_INV_SD);

	/* flush I-cache */
	cache_loop(start_addr, start_addr + size, ilsize, HIT_INVALIDATE_I);

ops_done:
	/* ensure cache ops complete before any further memory accesses */
	__sync();

	/* ensure the pipeline doesn't contain now-invalid instructions */
	instruction_hazard_barrier();
}

void flush_dcache_range(unsigned long start_addr, unsigned long stop) {
	unsigned long lsize = dcache_line_size();
	unsigned long slsize = scache_line_size();

	log_debug("start(%x) stop(%x)", start_addr, stop);

	/* end will be miscalculated when size is zero, so we return here */
	if (start_addr == stop) {
		return;
	}

	cache_loop(start_addr, stop, lsize, HIT_WRITEBACK_INV_D);

	/* flush L2 cache */
	cache_loop(start_addr, stop, slsize, HIT_WRITEBACK_INV_SD);

	/* ensure cache ops complete before any further memory accesses */
	__sync();
}

void invalidate_dcache_range(unsigned long start_addr, unsigned long stop) {
	unsigned long lsize = dcache_line_size();
	unsigned long slsize = scache_line_size();

	log_debug("start(%x) stop(%x)", start_addr, stop);

	/* end will be miscalculated when size is zero, so we return here */
	if (start_addr == stop) {
		return;
	}

	/* invalidate L2 cache */
	cache_loop(start_addr, stop, slsize, HIT_INVALIDATE_SD);

	cache_loop(start_addr, stop, lsize, HIT_INVALIDATE_D);

	/* ensure cache ops complete before any further memory accesses */
	__sync();
}

int dcache_status(void) {
	unsigned int cca = mips_read_c0_config() & CONF_CM_CMASK;
	return cca != CONF_CM_UNCACHED;
}

void dcache_enable(void) {
	log_error("Not supported!");
}

void dcache_disable(void) {
	/* change CCA to uncached */
	mips_change_c0_config(CONF_CM_CMASK, CONF_CM_UNCACHED);

	/* ensure the pipeline doesn't contain now-invalid instructions */
	instruction_hazard_barrier();
}
