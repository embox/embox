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
	__asm__ __volatile__ ("mrs %0, daif;\n\t"
		: "=r"(reg)
	);
	return reg;
}

static inline void set_daif(uint64_t reg) {
	__asm__ __volatile__ ("msr daif, %0; \n\t"
		:
		: "r"(reg)
	);
}

#define read_system_reg(name) ({ \
		volatile uint64_t reg; \
		asm volatile("mrs  %0, "#name";" \
			: "=r"(reg) \
		); \
		reg; \
	})

#define write_system_reg(name, reg) do { \
		asm volatile("msr "#name", %0;" \
			: : "r"(reg) \
		); \
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

#endif /* AARCH_HAL_REG_ */
