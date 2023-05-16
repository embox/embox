/**
 * @file qspi_erase.c
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 10.05.2023
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#if defined USE_STM32L475E_IOT01
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_qspi.h" // includes "../Components/mx25r6435f/mx25r6435f.h"
#define QSPI_ERASE_TIME MX25R6435F_SECTOR_ERASE_MAX_TIME

#elif defined USE_STM32F769I_DISCOVERY
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_qspi.h" // includes "../Components/mx25l512/mx25l512.h"
#define QSPI_ERASE_TIME MX25L512_SUBSECTOR_ERASE_MAX_TIME

#else
#error Unsupported platform
#endif

//#include <arm/cpu_cache.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

static void print_usage(void) {
	printf("Usage: qspi_erase [-h] [-t] [-s start] [-c count]\n");
}

static inline int poll_flash_busy(int maxms) {
	clock_t t0 = clock();
	while ((clock()-t0 < CLOCKS_PER_SEC*maxms/1000) && (BSP_QSPI_GetStatus() == QSPI_BUSY))
		usleep(1000);
	return (clock()-t0)*1000/CLOCKS_PER_SEC;
}

static int qspi_erase(int start, int count, int trace) {
	QSPI_Info info;
	
	BSP_QSPI_Init();
	int t=poll_flash_busy(100); t=clock();
	if (trace) printf("BSP_QSPI_Init... flash busy for %d ms\n", (clock()-t)*1000/CLOCKS_PER_SEC);

	BSP_QSPI_GetInfo(&info);

	for (int res, b = start; b < start + min(count, info.EraseSectorsNumber); b++) {
#if defined USE_STM32L475E_IOT01
		res = BSP_QSPI_Erase_Sector(b);
#elif defined USE_STM32F769I_DISCOVERY
		res = BSP_QSPI_Erase_Block(b * info.EraseSectorSize);
#else
#error Unsupported platform
#endif // defined USE_XXX

		if(res == QSPI_OK) { //usleep(QSPI_ERASE_TIME * 1000);
			int t=poll_flash_busy(QSPI_ERASE_TIME);
			if (trace) printf("Erased block %d (t=%d ms)\n", b, t);
		} else {
			printf("Fail erasing block %d, result=%d\n", b, res);
		}
	}

	BSP_QSPI_EnableMemoryMappedMode();

//	dcache_flush_all(); // - used in qspi_loader

	return 0;
}

int main(int argc, char **argv) {
	int opt;
	int start = 0, count = 0, trace = 0;

	while (-1 != (opt = getopt(argc, argv, "hts:c:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 't':
			trace = 1;
			break;
		case 's':
			start = strtol(optarg, NULL, 0);
			break;
		case 'c':
			count = strtol(optarg, NULL, 0);
			break;
		default:
			return 0;
		}
	}
	
	qspi_erase(start, count, trace);
	
	printf("OK\n");

	return 0;
}
