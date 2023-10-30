/**
 * @file reg.h
 * @brief Stub header
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */

#ifndef AARCH_HAL_REG_
#define AARCH_HAL_REG_

#include <stdint.h>

static inline uint64_t get_daif(void) {
	volatile uint64_t reg;
	__asm__ __volatile__("mrs %0, daif;\n\t" : "=r"(reg));
	return reg;
}

static inline void set_daif(uint64_t reg) {
	__asm__ __volatile__("msr daif, %0; \n\t" : : "r"(reg));
}

#define read_system_reg(name)              \
	({                                     \
		volatile uint64_t reg;             \
		asm volatile("mrs  %0, " #name ";" \
		             "isb;"                \
		             : "=r"(reg));         \
		reg;                               \
	})

#define write_system_reg(name, reg)               \
	do {                                          \
		__asm__ __volatile__("msr " #name ", %0;" \
		                     "isb;"               \
		                     :                    \
		                     : "r"(reg));         \
	} while (0)

static inline uint64_t icc_ctlr_el1_read(void) {
	return read_system_reg(S3_0_C12_C12_4);
}

static inline void icc_ctlr_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C12_4, reg);
}

static inline uint64_t icc_pmr_el1_read(void) {
	return read_system_reg(S3_0_C4_C6_0);
}

static inline void icc_pmr_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C4_C6_0, reg);
}

static inline uint64_t icc_iar0_el1_read(void) {
	return read_system_reg(S3_0_C12_C8_0);
}

static inline void icc_iar0_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C8_0, reg);
}

static inline uint64_t icc_iar1_el1_read(void) {
	return read_system_reg(S3_0_C12_C12_0);
}

static inline void icc_iar1_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C12_0, reg);
}

static inline uint64_t icc_igrpen0_el1_read(void) {
	return read_system_reg(S3_0_C12_C12_6);
}

static inline void icc_igrpen0_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C12_6, reg);
}

static inline uint64_t icc_igrpen1_el1_read(void) {
	return read_system_reg(S3_0_C12_C12_7);
}

static inline void icc_igrpen1_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C12_7, reg);
}

static inline uint64_t icc_igrpen1_el3_read(void) {
	return read_system_reg(S3_6_C12_C12_7);
}

static inline void icc_igrpen1_el3_write(uint64_t reg) {
	write_system_reg(S3_6_C12_C12_7, reg);
}

static inline uint64_t icc_eoir0_el1_read(void) {
	return read_system_reg(S3_0_C12_C8_1);
}

static inline void icc_eoir0_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C8_1, reg);
}

static inline uint64_t icc_eoir1_el1_read(void) {
	return read_system_reg(S3_0_C12_C12_1);
}

static inline void icc_eoir1_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C12_1, reg);
}

static inline uint64_t icc_sre_el1_read(void) {
	return read_system_reg(S3_0_C12_C12_5);
}

static inline void icc_sre_el1_write(uint64_t reg) {
	write_system_reg(S3_4_C12_C9_5, reg);
}

static inline uint64_t icc_sre_el2_read(void) {
	return read_system_reg(S3_4_C12_C9_5);
}

static inline void icc_sre_el2_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C12_5, reg);
}

static inline uint64_t icc_sre_el3_read(void) {
	return read_system_reg(S3_6_C12_C12_5);
}

static inline void icc_sre_el3_write(uint64_t reg) {
	write_system_reg(S3_6_C12_C12_5, reg);
}

static inline uint64_t icc_hppir0_el1_read(void) {
	return read_system_reg(S3_0_C12_C8_2);
}

static inline void icc_hppir0_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C8_2, reg);
}

static inline uint64_t icc_hppir1_el1_read(void) {
	return read_system_reg(S3_0_C12_C12_2);
}

static inline void icc_hppir1_el1_write(uint64_t reg) {
	write_system_reg(S3_0_C12_C12_2, reg);
}

static inline uint64_t aarch64_isr_el1(void) {
	return read_system_reg(ISR_EL1);
}

/* Raw CurrentEL register */
static inline uint64_t aarch64_current_el_read(void) {
	return read_system_reg(CurrentEL);
}

/* Convert CurrentEL to number from 0 to 3 */
static inline int aarch64_current_el(void) {
#define CURRENT_EL_OFFSET 2
#define CURRENT_EL_MASK   0xC
	return (int)((aarch64_current_el_read() & CURRENT_EL_MASK)
	             >> CURRENT_EL_OFFSET);
}

static inline uint64_t aarch64_hcr_el2_read(void) {
	return read_system_reg(HCR_EL2);
}

static inline uint64_t aarch64_ttbr0_el2_read(void) {
	return read_system_reg(TTBR0_EL2);
}

static inline void aarch64_ttbr0_el2_write(uint64_t reg) {
	write_system_reg(TTBR0_EL2, reg);
}

static inline uint64_t aarch64_ttbr0_el1_read(void) {
	return read_system_reg(TTBR0_EL1);
}

static inline void aarch64_ttbr0_el1_write(uint64_t reg) {
	write_system_reg(TTBR0_EL1, reg);
}

static inline uint64_t aarch64_ttbr1_el2_read(void) {
	return read_system_reg(TTBR1_EL2);
}

static inline void aarch64_hcr_el2_write(uint64_t reg) {
	write_system_reg(HCR_EL2, reg);
}

static inline uint64_t aarch64_sctlr_el2_read(void) {
	return read_system_reg(SCTLR_EL2);
}

static inline void aarch64_sctlr_el2_write(uint64_t reg) {
	write_system_reg(SCTLR_EL2, reg);
}

static inline uint64_t aarch64_sctlr_el1_read(void) {
	return read_system_reg(SCTLR_EL1);
}

static inline void aarch64_sctlr_el1_write(uint64_t reg) {
	write_system_reg(SCTLR_EL1, reg);
}

static inline uint64_t aarch64_esr_el2_read(void) {
	return read_system_reg(ESR_EL2);
}

static inline uint64_t aarch64_esr_el1_read(void) {
	return read_system_reg(ESR_EL1);
}

static inline uint64_t aarch64_spsr_el2_read(void) {
	return read_system_reg(SPSR_EL2);
}

static inline uint64_t aarch64_spsr_el1_read(void) {
	return read_system_reg(SPSR_EL1);
}

static inline uint64_t aarch64_far_el2_read(void) {
	return read_system_reg(FAR_EL2);
}

static inline void aarch64_far_el2_write(uint64_t reg) {
	write_system_reg(FAR_EL2, reg);
}

static inline uint64_t aarch64_far_el1_read(void) {
	return read_system_reg(FAR_EL1);
}

static inline void aarch64_far_el1_write(uint64_t reg) {
	write_system_reg(FAR_EL1, reg);
}

static inline uint64_t aarch64_tcr_el1_read(void) {
	return read_system_reg(TCR_EL1);
}

static inline void aarch64_tcr_el1_write(uint64_t reg) {
	write_system_reg(TCR_EL1, reg);
}

static inline uint64_t aarch64_tcr_el2_read(void) {
	return read_system_reg(TCR_EL2);
}

static inline void aarch64_tcr_el2_write(uint64_t reg) {
	write_system_reg(TCR_EL2, reg);
}

static inline uint64_t aarch64_id_aa64mmfr0_read(void) {
	return read_system_reg(ID_AA64MMFR0_EL1);
}

#endif /* AARCH_HAL_REG_ */
