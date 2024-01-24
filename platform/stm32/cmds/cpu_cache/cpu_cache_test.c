/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <hal/system.h>
#include <lib/libds/array.h>

#include <stm32f7xx_hal.h>
#include <asm/arm_m_regs.h>

#define DATA_IN_SDRAM

#define VALUE         0xc2
#define ITERS         100
#define BLOCK_LEN     4096
#define BLOCKS        16
#define DATA_LEN      (BLOCKS * BLOCK_LEN)
static_assert(DATA_LEN <= 64 * 1024, "");

#ifdef DATA_IN_SDRAM
#define DATA_ADDR     0x60100000
#else
#define DATA_ADDR     0x20040000
#endif

#define DEMCR        0xE000EDFC
# define DEMCR_TRCENA    0x01000000

#define DWT_CTRL     0xE0001000
# define CYCCNTENA   (1 << 0)

#define DWT_CYCCNT   0xE0001004

/* CoreSight Lock Access key, common for all */
#define DWT_LAR      0xE0001FB0
#define DWT_LAR_KEY  0xC5ACCE55

static void dwt_cyccnt_reset(void) {
	REG32_ORIN(DEMCR, DEMCR_TRCENA);

	REG32_STORE(DWT_LAR, DWT_LAR_KEY);

	REG32_STORE(DWT_CYCCNT, 0);
}

static inline uint32_t dwt_cyccnt_start(void) {
	REG32_ORIN(DWT_CTRL, CYCCNTENA);

	return REG32_LOAD(DWT_CYCCNT);
}

static inline uint32_t dwt_cyccnt_stop(void) {
	REG32_CLEAR(DWT_CTRL, CYCCNTENA);

	return REG32_LOAD(DWT_CYCCNT);
}

static void test1(void) {
	uint8_t register *dst;
	int register i, j;

	dst = (uint8_t *) DATA_ADDR;

	for (i = 0; i < ITERS * 8; i++) {
		for (j = 0; j < DATA_LEN; j++) {
			*dst = VALUE;
			dst++;
		}
		dst -= DATA_LEN;
	}
}

static void test2(void) {
	uint8_t register *dst;
	int register i, j;

	dst = (uint8_t *) DATA_ADDR;

	for (i = 0; i < ITERS * BLOCKS * 8; i++) {
		for (j = 0; j < BLOCK_LEN; j++) {
			*dst = VALUE;
			*dst = VALUE;
			*dst = VALUE;
			*dst = VALUE;
			dst++;
		}
		dst -= BLOCK_LEN;
	}
}

static void test3(void) {
	uint8_t register *dst;
	uint8_t register val;
	int register i, j;

	for (i = 0; i < ITERS * BLOCKS * 8; i++) {
		dst = (uint8_t *) DATA_ADDR;

		for (j = 0; j < BLOCK_LEN; j++) {
			val = VALUE;
			*dst = val;
			val = *dst;
			dst++;
		}
	}
}

static void test4(void) {
	uint8_t *arr;
	int i, j;

	arr = (uint8_t *) DATA_ADDR;

	for (i = 0; i < ITERS * BLOCKS; i++) {
		for (j = 0; j < BLOCK_LEN; j++) {
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
			arr[i]++;
		}
	}
}

static void test5(void) {
	uint8_t *arr;
	int i, j;

	arr = (uint8_t *) DATA_ADDR;

	for (i = 0; i < ITERS * BLOCKS; i++) {
		for (j = 0; j < BLOCK_LEN; j++) {
			arr[i + 0 ]++;
			arr[i + 1 ]++;
			arr[i + 2 ]++;
			arr[i + 3 ]++;
			arr[i + 4 ]++;
			arr[i + 100]++;
			arr[i + 6 ]++;
			arr[i + 7 ]++;
			arr[i + 8 ]++;
			arr[i + 9 ]++;
			arr[i + 10]++;
			arr[i + 11]++;
			arr[i + 12]++;
			arr[i + 13]++;
			arr[i + 14]++;
			arr[i + 15]++;
		}
	}
}

static void test6(void) {
	uint8_t *arr;
	int i, j;

	arr = (uint8_t *) DATA_ADDR;

	for (i = 0; i < ITERS * BLOCKS; i++) {
		for (j = 0; j < BLOCK_LEN; j++) {
			arr[i + 0 ]++;
			arr[i + 1 ]++;
			arr[i + 2 ]++;
			arr[i + 3 ]++;
			arr[i + 4 ]++;
			arr[i + 100]++;
			arr[i + 6 ]++;
			arr[i + 7 ]++;
			arr[i + 8 ]++;
			arr[i + 9 ]++;
			arr[i + 200]++;
			arr[i + 11]++;
			arr[i + 12]++;
			arr[i + 13]++;
			arr[i + 14]++;
			arr[i + 15]++;
		}
	}
}

static void test7(void) {
	uint8_t *arr;
	int i, j;

	arr = (uint8_t *) DATA_ADDR;

	for (i = 0; i < ITERS * BLOCKS; i++) {
		for (j = 0; j < BLOCK_LEN; j++) {
			arr[j + 0 ]  = VALUE;
			arr[j + 1 ]  = VALUE;
			arr[j + 2 ]  = VALUE;
			arr[j + 3 ]  = VALUE;
			arr[j + 4 ]  = VALUE;
			arr[j + 5 ]  = VALUE;
			arr[j + 6 ]  = VALUE;
			arr[j + 7 ]  = VALUE;
			arr[j + 8 ]  = arr[i % 1024 + (j % 256) * 128];
			arr[j + 9 ]  = VALUE;
			arr[j + 10 ]  = VALUE;
			arr[j + 11 ]  = VALUE;
			arr[j + 12 ]  = VALUE;
			arr[j + 13 ]  = VALUE;
			arr[j + 14 ]  = VALUE;
			arr[j + 15 ]  = VALUE;
		}
	}
}

static __attribute__((used)) uint8_t test8_var;

static void test8(void) {
	uint8_t *arr, *arr2;
	int i, j;

	arr = (uint8_t *) DATA_ADDR;
	arr2 = arr;

	for (i = 0; i < ITERS * BLOCKS; i++) {
		for (j = 0; j < BLOCK_LEN; j++) {
			arr2[i * BLOCK_LEN            ] = arr[j + 0 ];
			arr2[i * BLOCK_LEN + j*32 + 1 ] = arr[j + 1 ];
			arr2[i * BLOCK_LEN + j*64 + 2 ] = arr[j + 2 ];
			arr2[i * BLOCK_LEN + j*128 + 3] = arr[j + 3 ];
			arr2[i * BLOCK_LEN + j*32 + 4 ] = arr[j + 4 ];
			arr2[i * BLOCK_LEN + j*32 + 5 ] = arr[j + 5 ];
			arr2[i * BLOCK_LEN + j*32 + 6 ] = arr[j + 6 ];
			arr2[i * BLOCK_LEN + j*32 + 7 ] = arr[j + 7 ];
			arr2[i * BLOCK_LEN + j*32 + 8 ] = arr[j + 8 ];
			arr2[i * BLOCK_LEN + j*32 + 9 ] = arr[j + 9 ];
			arr2[i * BLOCK_LEN + j*32 + 10] = arr[j + 10 ];
			arr2[i * BLOCK_LEN + j*32 + 11] = arr[j + 11 ];
			arr2[i * BLOCK_LEN + j*32 + 12] = arr[j + 12 ];
			arr2[i * BLOCK_LEN + j*32 + 13] = arr[j + 13 ];
			arr2[i * BLOCK_LEN + j*32 + 14] = arr[j + 14 ];
			arr2[i * BLOCK_LEN + j*32 + 15] = arr[j + 15 ];
		}
	}

	test8_var = arr2[0];
}

static void test_time(void (*func)(void), const char *test_name) {
	uint32_t start, stop, res;

	SCB_CleanInvalidateDCache();

	dwt_cyccnt_reset();

	start = dwt_cyccnt_start();
	{
		func();
	}
	stop = dwt_cyccnt_stop();

	res = stop - start;

	printf("%s:\n  %ds %dms\n", test_name,
		res / SYS_CLOCK, (int) ((((uint64_t) res % SYS_CLOCK) * 1000) / SYS_CLOCK));
}

static void arm_mpu_configure(int region, uint32_t base_addr,
		uint32_t log_size, uint32_t attrs) {
	REG32_STORE(MPU_RNR, region);
	REG32_STORE(MPU_RBAR, base_addr);
	REG32_STORE(MPU_RASR, attrs | ((log_size - 1) << MPU_RASR_SIZE_POS));
}

static struct {
	void (*func)(void);
	const char *test_name;
} cpu_cache_tests[] = {
	{
		test1,
		"Test1 (Sequential write)",
	},
	{
		test2,
		"Test2 (Sequential write with 4 writes per one iteration)",
	},
	{
		test3,
		"Test3 (Sequential read/write)",
	},
	{
		test4,
		"Test4 (Array elements increment)",
	},
	{
		test5,
		"Test5 (Array elements increment with 1 sparse index)",
	},
	{
		test6,
		"Test6 (Array elements increment with 2 sparse indexes)",
	},
	{
		test7,
		"Test7 (Example where WB is better than WBNA (more writes))",
	},
	{
		test8,
		"Test8 (Example where WBNA is better than WB (more reads))",
	}
};

static void show_usage(void) {
	int i;

	printf("Usage: cpu_cache_tests -t [test_nr] [mode]\n");
	printf("  Example: cpu_cache_tests -t 1 wb\n\n");
	printf("Tests:\n");
	for (i = 0; i < ARRAY_SIZE(cpu_cache_tests); i++) {
		printf("  %d: %s\n", i + 1, cpu_cache_tests[i].test_name);
	}
}

int main(int argc, char **argv) {
	char *mode;
	int opt, i, test_nr = -1;

	while (-1 != (opt = getopt(argc, argv, "ht:"))) {
		switch (opt) {
		case 't':
			test_nr = strtol(optarg, NULL, 0) - 1;
			if (test_nr >= ARRAY_SIZE(cpu_cache_tests) || test_nr < 0) {
				fprintf(stderr, "Test number overflow\n");
				return 0;
			}
			break;
		case 'h':
		default:
			show_usage();
			return 0;
		}
	}

	if (argc < 2) {
		show_usage();
		return 0;
	}

	mode = argv[argc - 1];

	ARM_MPU_Disable();

	SCB_InvalidateICache();
	SCB_DisableICache();

	SCB_InvalidateDCache();
	SCB_DisableDCache();

	/* Create MPU Region number 8 with 64 Kb size, with defined attributes. */
	if (!strcmp(mode, "nc")) { /* non-cachable */
		ARM_MPU_Disable();
		arm_mpu_configure(8, (uint32_t) DATA_ADDR, 18,
			(1 << MPU_RASR_ENABLE_POS) | (0x3 << MPU_RASR_AP_POS) |
			(0x1 << MPU_RASR_TEX_POS));
		ARM_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

		SCB_DisableICache();
		SCB_EnableDCache();
	} else if (!strcmp(mode, "wb")) { /* write-back, write allocate */
		ARM_MPU_Disable();
		arm_mpu_configure(8, (uint32_t) DATA_ADDR, 18,
			(1 << MPU_RASR_ENABLE_POS) | (0x3 << MPU_RASR_AP_POS) |
			(0x1 << MPU_RASR_TEX_POS) | (1 << MPU_RASR_B_POS) | (1 << MPU_RASR_C_POS));
		ARM_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

		SCB_DisableICache();
		SCB_EnableDCache();
	} else if (!strcmp(mode, "wbna")) { /* write back, no write allocate */
		ARM_MPU_Disable();
		arm_mpu_configure(8, (uint32_t) DATA_ADDR, 18,
			(1 << MPU_RASR_ENABLE_POS) | (0x3 << MPU_RASR_AP_POS) |
			(0x0 << MPU_RASR_TEX_POS) | (1 << MPU_RASR_B_POS) | (1 << MPU_RASR_C_POS));
		ARM_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

		SCB_DisableICache();
		SCB_EnableDCache();
	} else if (!strcmp(mode, "wt")) { /* write through, no write allocate */
		ARM_MPU_Disable();
		arm_mpu_configure(8, (uint32_t) DATA_ADDR, 18,
			(1 << MPU_RASR_ENABLE_POS) | (0x3 << MPU_RASR_AP_POS) |
			(0x0 << MPU_RASR_TEX_POS) | (1 << MPU_RASR_C_POS));
		ARM_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

		SCB_DisableICache();
		SCB_EnableDCache();
	} else {
		fprintf(stderr, "Unknown mode.\n");
		fprintf(stderr, "  Avalilable modes:\n");
		fprintf(stderr, "    nc (nocache), wb (write-back), wt (write-through),\n");
		fprintf(stderr, "    wbna (write-back, no write allocate)\n");
		return 0;
	}

	printf("mode: %s, iters=%d, data len=%d, addr=0x%x\n",
		mode, ITERS, DATA_LEN, DATA_ADDR);

	ipl_disable();
	{
		if (test_nr >= 0) {
			test_time(cpu_cache_tests[test_nr].func,
				      cpu_cache_tests[test_nr].test_name);
		} else {
			for (i = 0; i < ARRAY_SIZE(cpu_cache_tests); i++) {
				test_time(cpu_cache_tests[i].func,
					      cpu_cache_tests[i].test_name);
			}
		}
	}
	ipl_enable();

	return 0;
}
