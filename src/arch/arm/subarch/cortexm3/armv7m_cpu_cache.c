/**
 * @file
 * @brief Enables I-Cache and D-Cache for ARMv7-M.
 *
 * @date    26.08.2020
 * @author  Alexander Kalmuk
 */

#include <stdint.h>
#include <assert.h>
#include <util/log.h>
#include <hal/reg.h>
#include <embox/unit.h>
#include <asm/arm_m_regs.h>

EMBOX_UNIT_INIT(arm_cpu_cache_init);

#define NOCACHE_REGION0_ADDR    OPTION_GET(NUMBER, nocache_region0_addr)
#define NOCACHE_REGION0_SIZE    OPTION_GET(NUMBER, nocache_region0_size)

#define NOCACHE_REGION1_ADDR    OPTION_GET(NUMBER, nocache_region1_addr)
#define NOCACHE_REGION1_SIZE    OPTION_GET(NUMBER, nocache_region1_size)

struct mpu_region {
	uint32_t addr;
	uint32_t size;
};

#define SCB_CCSIDR_SETS(ccsidr) \
	(((ccsidr) >> SCB_CCSIDR_SETS_POS) & SCB_CCSIDR_SETS_MASK)

#define SCB_CCSIDR_WAYS(ccsidr) \
	(((ccsidr) >> SCB_CCSIDR_WAYS_POS) & SCB_CCSIDR_WAYS_MASK)

static inline uint32_t calc_log2(uint32_t val) {
	return 31 - __builtin_clz(val);
}

extern char _sram_nocache_start;
extern char _sram_nocache_size;
extern char _sram_nocache_log_size;
extern char _sram_device_mem_start;
extern char _sram_device_mem_stop;

static inline void arm_mpu_disable(void) {
	dsb();

	REG64_CLEAR(SCB_SHCSR, SCB_SHCSR_MEMFAULTENA);

	REG32_STORE(MPU_CTRL, 0);
}

static inline void arm_mpu_enable(void) {
	REG32_STORE(MPU_CTRL, MPU_CTRL_PRIVDEFENA | MPU_CTRL_ENABLE);

	REG64_ORIN(SCB_SHCSR, SCB_SHCSR_MEMFAULTENA);

	dsb();
	isb();
}

static void arm_mpu_configure(int region, uint32_t base_addr,
		uint32_t log_size, uint32_t attrs) {
	REG32_STORE(MPU_RNR, region);
	REG32_STORE(MPU_RBAR, base_addr);
	REG32_STORE(MPU_RASR, attrs | ((log_size - 1) << MPU_RASR_SIZE_POS));
}

static void arm_mpu_init_nocache_regions(void) {
	int i, logsize;
	uint32_t sram_nocache_start = (uint32_t) &_sram_nocache_start;
	uint32_t sram_nocache_size = (uint32_t) &_sram_nocache_size;
	uint32_t sram_nocache_log_size = (uint32_t) &_sram_nocache_log_size;
	uint32_t sram_device_start = (uint32_t) &_sram_device_mem_start;
	uint32_t sram_device_stop = (uint32_t) &_sram_device_mem_stop;
	uint32_t sram_device_size = sram_device_stop - sram_device_start;

	unsigned region = 0;
	struct mpu_region regions[] = {
		{ NOCACHE_REGION0_ADDR, NOCACHE_REGION0_SIZE },
		{ NOCACHE_REGION1_ADDR, NOCACHE_REGION1_SIZE },
	};

	/* Process non-cacheble SRAM section */
	if (sram_nocache_size > 0) {
		log_debug("SRAM section non-cacheble: start=0x%08x, region size=0x%x",
			sram_nocache_start, sram_nocache_size);

		/* Non-cacheable region with full read/write access */
		arm_mpu_configure(region++, sram_nocache_start, sram_nocache_log_size,
			(1 << MPU_RASR_ENABLE_POS) | (0x3 << MPU_RASR_AP_POS) |
			(1 << MPU_RASR_TEX_POS));

		if (sram_device_size) {
			logsize = calc_log2(sram_device_size);
			if (sram_device_size != (1 << logsize)) {
				logsize++;
			}
			log_debug("SRAM device memory: start=0x%08x, region size=0x%x",
				sram_device_start, (1 << logsize));

			arm_mpu_configure(region++, sram_device_start, logsize,
				(1 << MPU_RASR_ENABLE_POS) | (0x3 << MPU_RASR_AP_POS) |
				(0 << MPU_RASR_TEX_POS) | (1 << MPU_RASR_B_POS));
		}
	}

	/* Process other non-cacheble regions at fixed addresses. */
	for (i = 0; i < ARRAY_SIZE(regions); i++) {
		if (!regions[i].size) {
			continue;
		}

		log_debug("Region%d non-cacheble: start=0x%08x, region size=0x%x",
			i, regions[i].addr, regions[i].size);

		logsize = calc_log2(regions[i].size);
		assertf(regions[i].size == (1 << logsize),
			"region%d size (0x%08x) is not a power of 2\n", i, regions[i].size);

		/* Non-cacheable region with full read/write access */
		arm_mpu_configure(region++, regions[i].addr, logsize,
			(1 << MPU_RASR_ENABLE_POS) | (0x3 << MPU_RASR_AP_POS) |
			(1 << MPU_RASR_TEX_POS));
	}
}

static void arm_cpu_icache_enable(void) {
	dsb();
	isb();

	/* Invalidate I-Cache */
	REG32_STORE(SCB_ICIALLU, 0);
	/* Enable I-Cache */
	REG32_ORIN(SCB_CCR, SCB_CCR_IC);

	dsb();
	isb();
}

static void arm_cpu_dcache_enable(void) {
	uint32_t ccsidr, sets, ways;

	/* Level 1 data cache */
	REG32_STORE(SCB_CSSELR, (0U << 1U) | 0U);
	dsb();

	ccsidr = REG32_LOAD(SCB_CCSIDR);

	/* Invalidate D-Cache */
	sets = SCB_CCSIDR_SETS(ccsidr);
	do {
		ways = SCB_CCSIDR_WAYS(ccsidr);
		do {
			REG32_STORE(SCB_DCISW, (sets << SCB_DCISW_SET_POS) |
	                               (ways << SCB_DCISW_WAY_POS));
		} while (ways--);
	} while (sets--);
	dsb();

	REG32_ORIN(SCB_CCR, SCB_CCR_DC);

	dsb();
	isb();
}

void dcache_flush_all(void) {
	uint32_t ccsidr, sets, ways;

	/* Level 1 data cache */
	REG32_STORE(SCB_CSSELR, (0U << 1U) | 0U);
	dsb();

	ccsidr = REG32_LOAD(SCB_CCSIDR);

	/* Clean D-Cache */
	sets = SCB_CCSIDR_SETS(ccsidr);
	do {
		ways = SCB_CCSIDR_WAYS(ccsidr);
		do {
			REG32_STORE(SCB_DCCSW, (sets << SCB_DCCSW_SET_POS) |
	                               (ways << SCB_DCCSW_WAY_POS));
		} while (ways--);
	} while (sets--);

	dsb();
	isb();
}

static int arm_cpu_cache_init(void) {
	arm_mpu_disable();
	arm_mpu_init_nocache_regions();
	arm_mpu_enable();

	arm_cpu_icache_enable();
	arm_cpu_dcache_enable();

	return 0;
}
