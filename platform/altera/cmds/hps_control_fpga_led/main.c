/**
 * @file main.c
 * @brief Control FPGA leds with HPS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 29.01.2020
 */
#include <stdio.h>
#include <unistd.h>

#include <mem/vmem.h>

#include "hps_0.h"
#include "led.h"

int main(int argc, char **argv) {
	vmem_map_region(vmem_current_context(),
			(mmu_paddr_t) ~MMU_PAGE_MASK & (ALT_LWFPGASLVS_OFST + LED_PIO_BASE),
			(mmu_vaddr_t) ~MMU_PAGE_MASK & (ALT_LWFPGASLVS_OFST + LED_PIO_BASE),
			HW_REGS_SPAN,
			PROT_READ | PROT_NOCACHE | PROT_WRITE);

	while (1) {
		int i;

		printf("LED ON\n");

		for (i = 0; i <= 8; i++) {
			LEDR_LightCount(i);
			usleep(100 * 1000);
		}

		printf("LED OFF\n");

		for (i = 0; i <= 8; i++) {
			LEDR_OffCount(i);
			usleep(100 * 1000);
		}
	}

	return 0;
}
