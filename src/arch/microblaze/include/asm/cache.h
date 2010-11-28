/**
 * @file
 * @brief Cache operations
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_CACHE_H__
#define MICROBLAZE_CACHE_H__

#include <asm/msr.h>

/*TODO may be put it in config file*/
#define CACHE_LINE_LENGTH    32

inline static void cache_set_ctrl_reg(unsigned int ctrl_reg) {
	/* stub*/
}

inline static void cache_refresh(void) {
	/* seems that nothing to do here:
	 * Microblaze always writes through cache */
}

inline static void cache_enable(void) {
	msr_set(MSR_ICE_MASK | MSR_DCE_MASK);
}

inline static void cache_disable(void) {
	msr_clr(MSR_ICE_MASK | MSR_DCE_MASK);
}

inline static void cache_instr_enable(void) {
	msr_set(MSR_ICE_MASK);
}

inline static void cache_instr_disable(void) {
	msr_clr(MSR_ICE_MASK);
}

inline static void cache_data_enable(void) {
	msr_set(MSR_DCE_MASK);
}

inline static void cache_data_disable(void) {
	msr_clr(MSR_DCE_MASK);
}

#endif /* MICROBLAZE_CACHE_H__*/
