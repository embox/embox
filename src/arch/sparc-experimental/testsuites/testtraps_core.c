/**
 * @file
 *
 * @data 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>

/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define TRAP_TABLE_SIZE 0x400


uint8_t tests_trap_table[TRAP_TABLE_SIZE];

int trap_fire(unsigned int trap_number) {
	__asm__ __volatile__ ("ta %0\n\t"::"i" (trap_number));

	return 0;
}
