/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <asm/msr.h>
#include <hal/mm/mmu_core.h>

void mmu_save_status(uint32_t *status) {
	register uint32_t msr;
	__asm__ __volatile__ ("mfs     %0, rmsr;\n\t"
			"andni   %0, r0, %2;\n\t"
			"swi   %0, %1, 0;\n\t" :
			"=r"(msr) :
			"r"(status), "i"(0x2000) :
			"memory" );
}

void mmu_restore_status(uint32_t *status) {
	register uint32_t msr, tmp;
	__asm__ __volatile__ ("lwi  %0, %1, 0;\n\t"
			"mfs     %0, rmsr;\n\t"
			"or   %0, r0, %2;\n\t"
			"mts   rmsr, %1, 0;\n\t" :
			"=r"(msr), "=&r"(tmp):
			"r"(status), "i"(0x2000) :
			"memory" );
}

void mmu_save_table(uint32_t *status) {

}

void mmu_restore_table(uint32_t *status) {

}

int mmu_enable(uint32_t *table) {
	register uint32_t msr;
	__asm__ __volatile__ ("mfs     %0, rmsr;\n\t"
			"or   %0, r0, %2;\n\t"
			"mts     rmsr, r2;\n\t" :
			"=r"(msr) :
			"i"(0x2000) :
			"memory" );
	return 0;
}

