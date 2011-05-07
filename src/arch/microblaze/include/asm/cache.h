/**
 * @file
 * @brief Cache operations
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_CACHE_H_
#define MICROBLAZE_CACHE_H_

#include <asm/msr.h>

/*TODO may be put it in config file*/
#define CACHE_LINE_LENGTH    32

static inline void cache_set_ctrl_reg(unsigned int ctrl_reg) {
	/* stub*/
}

static inline void cache_refresh(void) {
	/* seems that nothing to do here:
	 * Microblaze always writes through cache */
}

static inline void cache_enable(void) {
	msr_set(MSR_ICE_MASK | MSR_DCE_MASK);
}

static inline void cache_disable(void) {
	msr_clr(MSR_ICE_MASK | MSR_DCE_MASK);
}

static inline void cache_instr_enable(void) {
	msr_set(MSR_ICE_MASK);
}

static inline void cache_instr_disable(void) {
	msr_clr(MSR_ICE_MASK);
}

static inline void cache_data_enable(void) {
	msr_set(MSR_DCE_MASK);
}

static inline void cache_data_disable(void) {
	msr_clr(MSR_DCE_MASK);
}

static inline void icache_flush(void) {
	int volatile temp = 0;
	unsigned int volatile start = 0;
	unsigned int volatile end = 0x800; /*2k*/
	unsigned int volatile line_length = 0x32;

	__asm__ __volatile__ (
		" 1:	wic	%1, r0;\n\t"
		"cmpu	%0, %1, %2;\n\t"
		"bgtid	%0, 1b;\n\t"
		"addk	%1, %1, %3;" : :
		"r" (temp), "r" (start), "r" (end), "r" (line_length)
		: "memory"
	);
}

static inline void dcache_flush(void) {
	int volatile temp = 0;
	unsigned int volatile start = 0;
	unsigned int volatile end = 0x800; /*2k*/
	unsigned int volatile line_length = 0x32;

	__asm__ __volatile__ (
		" 1:	wdc	%1, r0;\n\t"
		"cmpu	%0, %1, %2;\n\t"
		"bgtid	%0, 1b;\n\t"
		"addk	%1, %1, %3;" : :
		"r" (temp), "r" (start), "r" (end), "r" (line_length)
		: "memory"
	);
}

static inline void cache_flush(void) {
	icache_flush();
	dcache_flush();
}

#endif /* MICROBLAZE_CACHE_H_ */
