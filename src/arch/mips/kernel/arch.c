/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#include <hal/arch.h>

extern void mips_exception_init(void);

void arch_init(void) {
	mips_exception_init();
}
#include <asm/mipsregs.h>
#include <stdio.h>
void arch_idle(void) {
	unsigned int count;
	unsigned int compare;
	count =  read_c0_count();
	compare = read_c0_compare();
	if(count>compare) {
		//write_c0_compare(count + 10000);
	}
	printf("count %d compare %d\n", count, compare);
	for(count = 0; count < 1000000; count++) {

	}

}

void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
