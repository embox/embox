/**
 * @file
 *
 * @date 04.07.2010
 * @author Anton Bondarev
 */

#ifndef MICROMLAZE_OPEN_PROM_H_
#define MICROBLAZE_OPEN_PROM_H_

#include <open_prom.h>

#define LOAD_ADDR 0x44000000

typedef struct cpu_open_prom {
	struct linux_romvec romvec;
} cpu_open_prom_t;

//extern struct cpu_open_prom spi;

extern void bootm_linux(unsigned int load_addr, unsigned int entry_point);

#endif /* MICROMLAZE_OPEN_PROM_H_ */
