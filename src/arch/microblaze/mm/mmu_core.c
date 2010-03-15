/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <hal/mm/mmu_core.h>

void mmu_save_status(uint32_t *status) {
	uint32_t tmp;
	__asm__ __volatile__ ("mfs     %0, rmsr;\n\t"
			"andni   %0, r0, %2;\n\t"
			"swi   %0, %1, 0;\n\t" :
			"=&r"(tmp) :
			"r"(status), "i"(0x2000) :
			"memory" );
}

void mmu_restore_status(uint32_t *status) {
	uint32_t tmp, tmp1;
	__asm__ __volatile__ ("lwi  %0, %1, 0;\n\t"
			"mfs     %0, rmsr;\n\t"
			"or   %0, r0, %2;\n\t"
			"swi   %0, %1, 0;\n\t" :
			"=&r"(tmp), "=&r"(tmp):
			"r"(status), "i"(0x2000) :
			"memory" );
}

void mmu_save_table(uint32_t *status) {

}

void mmu_restore_table(uint32_t *status) {

}

int mmu_enable(uint32_t *table) {
	__asm__ __volatile__ ("mfs     %0, rmsr;\n\t"
			"or   %0, r0, %2;\n\t"
			"mts     rmsr, r2;\n\t" :
			"=&r"(tmp) :
			"i"(0x2000) :
			"memory" );
	return 0;
}

