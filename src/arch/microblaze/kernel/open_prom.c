/**
 * @file
 *
 * @date 04.07.2010
 * @author Anton Bondarev
 */
#include <asm/prom.h>

//struct cpu_open_prom spi;

void bootm_linux(unsigned int load_addr, unsigned int entry_point) {
	void (*kernel)(void);

	/* call kernel */
	kernel = (void (*)(void))entry_point;
	kernel();
}

