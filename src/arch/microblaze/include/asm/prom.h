/**
 * @file
 *
 * @date 04.07.2010
 * @author Anton Bondarev
 */

#ifndef MICROMLAZE_OPEN_PROM_H_
#define MICROBLAZE_OPEN_PROM_H_

#include <open_prom.h>

typedef struct cpu_open_prom {
	struct linux_romvec romvec;
}cpu_open_prom_t;

extern struct cpu_open_prom spi;

#endif /* MICROMLAZE_OPEN_PROM_H_ */
