/**
 * @file
 *
 * @brief Implementation of the RISC-V Core Local Interruptor (CLINT) for interrupt control, including support for SiFive multi-hart architectures.
 *
 * @date 05.07.2024
 * @authored by Suraj Ravindra Sonawane
 */

#include <asm/interrupts.h>
#include <drivers/irqctrl.h>
#include <hal/cpu.h>
#include <hal/reg.h>

#define HAS64BITMMIO	OPTION_GET(BOOLEAN, has64bitmmio)
#define CLINT_ADDR      OPTION_GET(NUMBER, base_addr)
#define MSIP_OFFSET     OPTION_GET(NUMBER, msip_offset)
#define MTIMECMP_OFFSET OPTION_GET(NUMBER, mtimecmp_offset)
#define MTIME_OFFSET    OPTION_GET(NUMBER, mtime_offset)

#define MSIP_ADDR(hart)     (CLINT_ADDR + MSIP_OFFSET + ((hart) * 4))
#define MTIMECMP_ADDR(hart) (CLINT_ADDR + MTIMECMP_OFFSET + ((hart) * 8))

/* time counter register is read only*/
#define MTIME_ADDR (CLINT_ADDR + MTIME_OFFSET)

#if __riscv_xlen != 32 && !HAS64BITMMIO

#define READ_64BIT(addr) ({		\
	uint32_t lo, hi;		\
	lo = REG32_LOAD(addr);		\
	hi = REG32_LOAD(addr + 4);	\
	(((uint64_t)hi << 32) | lo);	\
})

#define WRITE_64BIT(addr, value) do {			\
	REG32_STORE(addr, value & 0xFFFFFFFF); \
	REG32_STORE(addr + 4, (value >> 32) & 0xFFFFFFFF); \
} while(0)

#else /* __riscv_xlen == 32 || !HAS64BITMMIO */

#define WRITE_64BIT(addr, value)	REG64_STORE(addr, value)
#define READ_64BIT(addr)		REG64_LOAD(addr)

#endif /* __riscv_xlen == 32 || !HAS64BITMMIO */

/**
 * Initializes the CLINT.
 *
 * This function initializes the CLINT by clearing the MSIP (Software Interrupt) and setting MTIMECMP
 * to its maximum value (0xFFFFFFFFFFFFFFFF).
 *
 * @return 0 on success.
 */

/* In SMP case, each CPU will call this function independently */
int clint_init(void) {
	int hartid = cpu_get_id();
	REG32_STORE(MSIP_ADDR(hartid), 0); /* Clear Pending bit */
	WRITE_64BIT(MTIMECMP_ADDR(hartid), 0xFFFFFFFFFFFFFFFFU);

	enable_software_interrupts();
	return 0;
}

/**
 * Configures the Software Interrupt (MSIP).
 *
 * This function configures the MSIP by writing a specific value (0 or 1) to its address.
 *
 * @param value The value (0 or 1) to set for MSIP.
 * @param hart_id The hart id (only for SiFive CLINT).
 */
void clint_configure_msip(uint8_t value, int hart_id) {
	REG32_STORE(MSIP_ADDR(hart_id), value & 1);
}

/**
 * Sets the MTIMECMP register value.
 *
 * This function sets the MTIMECMP register to the provided 64-bit value.
 *
 * @param value The value to set for MTIMECMP.
 * @param hart_id The hart id (only for SiFive CLINT).
 */
void clint_set_mtimecmp(uint64_t value, int hart_id) {
	WRITE_64BIT(MTIMECMP_ADDR(hart_id), value);
}

/**
 * Sets the MTIMECMP register value.
 *
 * This function gets the MTIMECMP register to the provided 64-bit value.
 *
 * @param hart_id The hart id (only for SiFive CLINT).
 */
uint64_t clint_get_mtimecmp(int hart_id) {
	return READ_64BIT(MTIMECMP_ADDR(hart_id));
}

/**
 * Retrieves the current value of MTIME.
 *
 * This function reads and returns the current value of MTIME.
 *
 * @return The current value of MTIME.
 */
uint64_t clint_get_mtime(void) {
	return READ_64BIT(MTIME_ADDR);
}
