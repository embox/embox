/**
 * @file
 *
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for Microblaze architecture.
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
#include <asm/msr.h>
#include <hal/mm/mmu_core.h>

/* main system MMU environment*/
static mmu_env_t system_env;
static mmu_env_t *cur_env;

static inline void mmu_save_status(uint32_t *status) {
	register uint32_t msr;
	__asm__ __volatile__ ("mfs     %0, rmsr;\n\t"
			"andni   %0, r0, %2;\n\t"
			"swi   %0, %1, 0;\n\t" :
			"=r"(msr) :
			"r"(status), "i"(0x2000) :
			"memory" );
}

static inline void mmu_restore_status(uint32_t *status) {
	register uint32_t msr, tmp;
	__asm__ __volatile__ ("lwi  %0, %1, 0;\n\t"
			"mfs     %0, rmsr;\n\t"
			"or   %0, r0, %2;\n\t"
			"mts   rmsr, %1, 0;\n\t" :
			"=r"(msr), "=&r"(tmp):
			"r"(status), "i"(0x2000) :
			"memory" );
}

static inline void mmu_save_table(uint32_t *table) {

}

static inline void mmu_restore_table(uint32_t *table) {

}

void mmu_on(void) {
/*	register uint32_t msr;
	__asm__ __volatile__ ("mfs     %0, rmsr;\n\t"
			"or   %0, r0, %1;\n\t"
			"mts     rmsr, %0;\n\t" :
			"=r"(msr) :
			"i"(0x2000) :
			"memory" );*/
}

void mmu_off(void) {
/*	register uint32_t msr;
	__asm__ __volatile__ ("mfs     %0, rmsr;\n\t"
			"andni   %0, r0, %1;\n\t"
			"mts     rmsr, %0;\n\t" :
			"=r"(msr) :
			"i"(0x2000) :
			"memory" );*/
}

static inline void set_utlb_record(int tlbx, uint32_t tlblo, uint32_t tlbhi) {
/*	__asm__ __volatile__ ("mts rtlbx, %0;\n\t"
			"mts rtlblo, %1;\n\t"
			"mts rtlbhi, %2;\n\t"
			: "=r"(tlbx), "=r"(tlblo),"=r" (tlbhi)
			:
			: "memory" );*/
}

int mmu_map_region(uint32_t phy_addr, uint32_t virt_addr,
		size_t reg_size, uint32_t flags) {
	return -1;
}

void mmu_restore_env(mmu_env_t *env) {
//	unsigned int ipl = ipl_save();

	/* disable virtual mode*/
	mmu_off();

	/* copy utlb records*/

	/* change cur_env pointer */
	cur_env = env;

	/* restore MMU mode */
	env->status ? mmu_on(): mmu_off();

//	ipl_restore(ipl);
}

void mmu_save_env(mmu_env_t *env) {
//	unsigned int ipl = ipl_save();

	/* disable virtual mode*/
	mmu_off();

	/* copy utlb records*/

	/* change cur_env pointer */
	cur_env = env;

	/* restore MMU mode */
	env->status ? mmu_on(): mmu_off();

//	ipl_restore(ipl);
}

void mmu_set_env(mmu_env_t *env) {

}
